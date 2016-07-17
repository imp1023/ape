#define _CRT_SECURE_NO_WARNINGS
#include "WorldNetHandler.h"

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
#include "WebServerHandler.h"

WorldNetHandler::WorldNetHandler(EventQueue *eq, int wid): eq_(eq), nid_(wid)
{
	logger_ = log4cxx::Logger::getLogger("WorldNetHanlder");
}

bool WorldNetHandler::isListenSocket(int fd)
{
	return fd==gamedfd || fd==wsfd;
}

int WorldNetHandler::readCacheSize(int listenFd)
{
	return DEFAULT_SERVER_SIZE;
}

void WorldNetHandler::createProtocolHandler(NetCache *cache, int listenFd)
{
	int nfd = cache->fd;
	if (listenFd==gamedfd)	{
		cache->ph = new GameHandler(this, nfd);
		LOG4CXX_INFO(logger_, "Gamed connected from "<<cache->addrstr()<<" into fd:"<<nfd);
	} else if (listenFd==wsfd)	{
		cache->ph = new WebServerHandler(eq_, nfd, nid_);
		LOG4CXX_INFO(logger_, "Web Server connected from "<<cache->addrstr()<<" into fd:"<<nfd);
	}
}

int WorldNetHandler::initSockets()
{
	int port = ServerConfig::Instance().worlddGamedPort(nid_);
	string addr = ServerConfig::Instance().worlddIp(nid_);
	gamedfd = createListenSocket("gamed", toString(port), addr);
	if (gamedfd<0) {
		return -1;
	}

	port = ServerConfig::Instance().worlddWebPort(nid_);
	addr = ServerConfig::Instance().worlddIp(nid_);
	wsfd = createListenSocket("web", toString(port), addr);
	if (wsfd<0) {
		return -1;
	}
	last_keepalive_check = 0;
#ifdef _WIN32 // for windows
	return max(gamedfd, wsfd);
#else // for linux
	return 0;
#endif
}

bool WorldNetHandler::sendEventToGamed(Event *e, int gid) {
	int fd = getGamedFdFromGamedId(gid>=0? gid: 1);
	return sendEventToGamedWithFd(e, fd);
}

bool WorldNetHandler::sendEventToGamedWithFd(Event *e, int fd)
{
    if (fd<0)  {
        LOG4CXX_ERROR(logger_, "Get Fd from ID failed!!"<<" fd:"<<fd<<" cmd:"<<e->cmd());
        return false;
    }
    string text;
    e->SerializeToString(&text);
    text = "ev,"+text;
    bool succ = sendIntSizedFdString(fd, text);
    if (!succ) {
        LOG4CXX_ERROR(logger_, "Write to cache failed!!");
    }
    return succ;
}

bool WorldNetHandler::sendEventToAllGamed(Event *e)
{
	bool succ = true;
	for (int i=1; i<=ServerConfig::Instance().gamedNum(); i++) {
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

int64 WorldNetHandler::getIdFromFd(int fd)
{
	NetCache *cache = getCacheFromFd(fd);
	return (cache==NULL ? -1 : cache->uid);
}

int WorldNetHandler::getGamedFdFromGamedId(int gameid)
{
	map<int, int>::const_iterator it = idFd_.find(gameid);
	if (it!=idFd_.end()) return it->second;
	return -1;
}

void WorldNetHandler::removeConnection(int fd)
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

int WorldNetHandler::getFdFromId(int id)
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

bool WorldNetHandler::registerConnection(int fd, int id) {
	int cfd = getFdFromId(id);
	if (cfd>=0)	closeConnection(cfd);
	NetCache *cache = getCacheFromFd(fd);
	if (cache==NULL) return false;
	cache->uid = id;
	idFd_[id] = fd;
	LOG4CXX_INFO(logger_, "Gamed #"<<id<<" registered.");
	return true;
}

bool WorldNetHandler::preNetEvent(time_t now) {
	keepAliveWithGame(now);
	return true;
}

void WorldNetHandler::keepAliveWithGame(time_t now) {
	if ((now-last_keepalive_check)>30000) {
		last_keepalive_check = now;
		for (int i=1; i<=ServerConfig::Instance().gamedNum(); i++) {
			int fd = getFdFromId(i);
			if (fd>0) {
				sendIntSizedFdString(fd, "ka");
			}
		}
	}
}
