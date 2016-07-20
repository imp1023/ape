#define _CRT_SECURE_NO_WARNINGS
#include "PlatNetHandler.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

#include "../common/string-util.h"
#include "../event/EventQueue.h"
#include "GameHandler.h"
extern ServerConfig serverConfig;

PlatNetHandler::PlatNetHandler(EventQueue *eq,int nid)
: eq_(eq),nid_(nid)
{
	logger_ = log4cxx::Logger::getLogger("PlatNetHanlder");
}

bool PlatNetHandler::isListenSocket(int fd)
{
	return fd==gamedfd;
}

int PlatNetHandler::readCacheSize(int listenFd)
{
	return DEFAULT_SERVER_SIZE;
}

void PlatNetHandler::createProtocolHandler(NetCache *cache, int listenFd)
{
	int nfd = cache->fd;
	if (listenFd==gamedfd)	{
		cache->ph = new GameHandler(this, nfd);
		LOG4CXX_INFO(logger_, "Gamed connected from "<<cache->addrstr()<<" into fd:"<<nfd);
	}
}

extern int G_PlatSrvD_ID;

int PlatNetHandler::initSockets()
{
	int port = serverConfig.plat_gamed_Port(G_PlatSrvD_ID);
	string addr = serverConfig.plat_bind_ip(G_PlatSrvD_ID);
	gamedfd = createListenSocket("plat", toString(port), addr);
	if (gamedfd<0) {
		return -1;
	}

	last_keepalive_check = 0;
#ifdef _WIN32 // for windows
	return gamedfd;
#else // for linux
	return 0;
#endif
}

bool PlatNetHandler::sendEventToGamed(Event *e, int gid) {
	int fd = getGamedFdFromGamedId(gid>=0? gid: 1);
	//LOG4CXX_DEBUG(logger_, "look up gameid:"<<gid<<", get fd:"<<fd);
	if (fd<0)  {
		LOG4CXX_ERROR(logger_, "Get Fd from ID failed!! gid:"<<gid<<" fd:"<<fd<<" cmd:"<<e->cmd());
		return false;
	}
	e->set_starsrvid(nid_);
	string text;
	e->SerializeToString(&text);
	text = "ev,"+text;
	//LOG4CXX_DEBUG(logger_, "event:"<<e->DebugString()<<"to fd:"<<fd<<" with size:"<<text.length());
	//LOG4CXX_INFO(logger_, "send event " << e->cmd() << " to gamed fd " << fd << ", size: " << text.size() << ", 0 at " << text.find((char)0, 0));
	bool succ = sendIntSizedFdString(fd, text);
	//LOG4CXX_DEBUG(logger_, "sendEventToGamed, event: " << e->cmd() << ", size: " << text.size() << ", target fd: " << fd << ", write to cache: " << succ);
	if (!succ) {
		LOG4CXX_ERROR(logger_, "Write to cache failed!!");
	}
	return succ;
}

bool  PlatNetHandler::sendEvnetToOneGamed(Event *e)
{
	bool succ = false;
	for (int i=1; i<=serverConfig.gamedNum(); i++) {
		int fd = getFdFromId(i);
		if (fd<0) {
			LOG4CXX_ERROR(logger_, "Get Fd from ID failed!! gamed:"<<i<<" fd:"<<" cmd:"<<e->cmd());
			continue;
		}
		string text;
		e->SerializeToString(&text);
		text = "ev,"+text;
		//LOG4CXX_DEBUG(logger_, "event:"<<e->DebugString()<<"to fd:"<<fd<<" with size:"<<text.length());
		succ = sendIntSizedFdString(fd, text);
		if (!succ) {
			LOG4CXX_ERROR(logger_, "Write to cache failed!!");
		}
		else
		{
			break;
		}
		//LOG4CXX_DEBUG(logger_, "sendEventToAllGamed, event: " << e->cmd() << ", size: " << text.size() << ", target fd: " << fd << ", write to cache: " << sendSucc);
	}
	return succ;
}

bool PlatNetHandler::sendEventToAllGamed(Event *e)
{
	bool succ = true;
	for (int i=1; i<=serverConfig.gamedNum(); i++) {
		int fd = getFdFromId(i);
		if (fd<0) {
			LOG4CXX_ERROR(logger_, "Get Fd from ID failed!! gamed:"<<i<<" fd:"<<" cmd:"<<e->cmd());
			continue;
		}
		string text;
		e->SerializeToString(&text);
		text = "ev,"+text;
		//LOG4CXX_DEBUG(logger_, "event:"<<e->DebugString()<<"to fd:"<<fd<<" with size:"<<text.length());
		bool sendSucc = sendIntSizedFdString(fd, text);
		if (!sendSucc) {
			LOG4CXX_ERROR(logger_, "Write to cache failed!!");
		}
		//LOG4CXX_DEBUG(logger_, "sendEventToAllGamed, event: " << e->cmd() << ", size: " << text.size() << ", target fd: " << fd << ", write to cache: " << sendSucc);
		succ = succ && sendSucc;
	}
	return succ;
}

int64 PlatNetHandler::getIdFromFd(int fd)
{
	NetCache *cache = getCacheFromFd(fd);
	return (cache==NULL ? -1 : cache->uid);
}

int PlatNetHandler::getGamedFdFromGamedId(int gameid)
{
	// TODO: gamed selection algorithm
	map<int, int>::const_iterator it = idFd_.find(gameid);
	if (it!=idFd_.end()) return it->second;
	return -1;
}

void PlatNetHandler::removeConnection(int fd)
{
	NetCache *cache = getCacheFromFd(fd);
	if (cache!=NULL)
	{
		int id = (int)cache->uid;
		if (getFdFromId(id)==fd)
		{
			idFd_.erase(id);
		}
		NetHandler::removeConnection(fd);
	}
}

int PlatNetHandler::getFdFromId(int id)
{
	map<int,int>::const_iterator it = idFd_.find(id);
	if (it!=idFd_.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}

bool PlatNetHandler::registerConnection(int fd, int id) {
	int cfd = getFdFromId(id);
	if (cfd>=0)	closeConnection(cfd);
	NetCache *cache = getCacheFromFd(fd);
	if (cache==NULL) return false;
	cache->uid = id;
	idFd_[id] = fd;
	LOG4CXX_INFO(logger_, "Gamed #"<<id<<" registered.");
	return true;
}

bool PlatNetHandler::preNetEvent(time_t now) {
	keepAliveWithGame(now);
	return true;
}

void PlatNetHandler::keepAliveWithGame(time_t now) {
	if ((now-last_keepalive_check)>30000) {
		last_keepalive_check = now;
		for (int i=1; i<=serverConfig.gamedNum(); i++) {
			int fd = getFdFromId(i);
			if (fd>0) {
				sendIntSizedFdString(fd, "ka");
			}
		}
	}
}
