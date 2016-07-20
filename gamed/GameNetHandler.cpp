#define _CRT_SECURE_NO_WARNINGS
#include "GameNetHandler.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../event/event.pb.h"
#include "../event/EventQueue.h"
#include "../net/NetCache.h"
#include "../common/Clock.h"
#include "../common/string-util.h"
#include "./event/UpdateWorkingStatus.h"
#include "AdminHandler.h"
#include "ClientHandler.h"
#include "PolicyHandler.h"
#include "WorldHandler.h"
#include "PlatHandler.h"
#include "CountryHandler.h"
#include "../common/SysLog.h"
#include "../logic/GameSrvCfg.h"

extern ServerConfig serverConfig;

GameNetHandler::GameNetHandler(EventQueue *eventQueue, int nid)
: eq(eventQueue), nid_(nid)
{
	logger_ = log4cxx::Logger::getLogger("GameNetHandler");
	pthread_mutex_init(&kick_mutex, NULL);
	pthread_mutex_init(&join_mutex, NULL);
}

void GameNetHandler::addUser(int fd, int64 uid)
{
	lockjoin();
	map<int, int64>::iterator itor = mapJoinFd.find(fd);
	if(itor == mapJoinFd.end())
	{
		mapJoinFd.insert(make_pair(fd, uid));
	}
	else
	{
		mapJoinFd[fd] = uid;
	}
	unlockjoin();
}

void GameNetHandler::kickUser(int fd, int64 uid)
{
	lockkick();
	map<int, int64>::iterator itor = mapKickFd.find(fd);
	if(itor == mapKickFd.end())
	{
		mapKickFd.insert(make_pair(fd, uid));
	}
	else
	{
		mapKickFd[fd] = uid;
	}
	unlockkick();
}

// bool GameNetHandler::sendToTransfer(int nTransferID,const string& str)
// {
// 	return sendToServer(nTransferID,CST_Transfer,str);
// }

bool GameNetHandler::sendToPlat(int nPlatSrvID,const string& str)
{	
	return sendToServer(nPlatSrvID,CST_Plat,str);
}

// bool GameNetHandler::sendToCountry(int nCountrySrvID,const string& str)
// {
// 	return sendToServer(nCountrySrvID,CST_Country,str);
// }
// 
// bool GameNetHandler::sendToRegion(int nRegionSrvID,const string& str)
// {
// 	return sendToServer(nRegionSrvID,CST_Region,str);
// }
// 
// bool GameNetHandler::sendToRank(int nSrvID,const string& str)
// {
// 	return sendToServer(nSrvID,CST_Rank,str);
// }
// 
// bool GameNetHandler::sendToStat(int nSrvID,const string& str)
// {
// 	return sendToServer(nSrvID,CST_Stat,str);
// }
// 
// bool GameNetHandler::sendToFight(int nFightSrvID,const string& str)
// {	
// 	return sendToServer(nFightSrvID,CST_Fight,str);
// }
// 
// bool GameNetHandler::sendToStar(int nStarSrvID,const string& str)
// {	
// 	return sendToServer(nStarSrvID,CST_Star,str);
// }
// bool GameNetHandler::sendToMonitor(int nMonitorSrvID,const string& str)
// {
// 	return sendToServer(nMonitorSrvID,CST_Monitor,str);
// }

bool GameNetHandler::sendToServer(int nSrvID,ConnectSrvType emType,const string& str)
{
	if(emType < CST_World || emType >= CST_Cnt)
		return false;

	for(size_t i=0;i<m_ServerLst[emType].size();i++)
	{
		if(m_ServerLst[emType][i].id == nSrvID&&m_ServerLst[emType][i].Status == NET_ACTIVE)
		{
			bool succ = sendIntSizedFdString(m_ServerLst[emType][i].iFd, str);
			return succ;
		}
	}
	return false;
}
bool GameNetHandler::sendToWorld(int nWorldSrvID,const string& str)
{
	if(m_ServerLst[CST_World].size()<=0)
		return false;

	int fd = 0;
	if(nWorldSrvID>0)
	{
		for(size_t i=0;i<m_ServerLst[CST_World].size();i++)
		{
			if(m_ServerLst[CST_World][i].id == nWorldSrvID&&m_ServerLst[CST_World][i].Status == NET_ACTIVE)
			{
				fd = m_ServerLst[CST_World][i].iFd;
				break;
			}
		}
	}
	if(fd<=0)
	{
		int nIdx = nid_ % m_ServerLst[CST_World].size();
		for(size_t i = 0; i < m_ServerLst[CST_World].size(); i++ )
		{
			if(m_ServerLst[CST_World][nIdx].Status == NET_ACTIVE)
			{
				fd = m_ServerLst[CST_World][i].iFd;
				break;
			}
			nIdx ++; 
			if(nIdx >= (int)m_ServerLst[CST_World].size())
			{
				nIdx = 0;
			}
		}
	}
	return sendIntSizedFdString(fd, str);
}

bool GameNetHandler::sendDataToClient(int fd, int type, const string& str,bool bUseRc4)
{
	if(fd > 0)
	{
		return sendSizedTypeFdString(fd, type, str,bUseRc4);
	}
	return false;
}

bool GameNetHandler::isListenSocket(int fd)
{
	return fd==clientfd || fd==adminfd || fd==policyfd;
}

bool GameNetHandler::isConnectSocket(int fd)
{
	for(int j=CST_Begin;j<CST_Cnt;j++)
	{
		for (size_t i = 0; i < m_ServerLst[j].size(); i++)
		{
			if ( m_ServerLst[j][i].iFd == fd )
			{
				return true;
			}
		}
	}
 
	return false;
}
bool GameNetHandler::isConnecting(int fd)
{
	for(int j=CST_Begin;j<CST_Cnt;j++)
	{
		for (size_t i = 0; i < m_ServerLst[j].size(); i++)
		{
			if ( m_ServerLst[j][i].iFd == fd )
			{
				if ( m_ServerLst[j][i].Status == NET_CONNECTING )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
    return false;
}

bool GameNetHandler::connectSuccess(int fd)
{
	for(int j=CST_Begin;j<CST_Cnt;j++)
	{
		for(size_t i = 0; i<m_ServerLst[j].size(); i++)
		{
			if(m_ServerLst[j][i].iFd == fd)
			{
				// register to world server
				char buf[16];
				sprintf(buf, "a,%d", nid_);
				if(!sendIntSizedFdString(fd, string(buf)))
				{
					LOG4CXX_ERROR(logger_, "Send register request (#"<<nid_<<") to "<<GetSrvName(j)<<" failed.");
				}
				m_ServerLst[j][i].Status = NET_ACTIVE;
				return true;
			}
		}
	}

    return true;
}

const char* GameNetHandler::GetSrvName(int emType)
{
	switch(emType)
	{
	case CST_World:
		{
			return "World";
		}
		break;
// 	case CST_Star:
// 		{
// 			return "Star";
// 		}
// 		break;
// 	case CST_Country:
// 		{
// 			return "Country";
// 		}
// 		break;
	case CST_Plat:
		{
			return "Plat";
		}
		break;
// 	case CST_Fight:
// 		{
// 			return "Fight";
// 		}
// 		break;
// 	case CST_Region:
// 		{
// 			return "Region";
// 		}
// 		break;
// 	case CST_Transfer:
// 		{
// 			return "Transfer";
// 		}
// 		break;
// 	case CST_Monitor:
// 		{
// 			return "Monitor";
// 		}
// 		break;
// 	case CST_Rank:
// 		{
// 			return "Rank";
// 		}
// 		break;
// 	case CST_Stat:
// 		{
// 			return "Stat";
// 		}
		break;
	default:
		{
			return "Unknow";
		}
		break;
	}
}

int GameNetHandler::readCacheSize(int listenFd)
{
	return isConnectSocket(listenFd) ? DEFAULT_SERVER_SIZE
		: DEFAULT_CLIENT_SIZE;
}

void GameNetHandler::createProtocolHandler(NetCache *cache, int listenFd)
{
	int nfd = cache->fd;
	if (listenFd==adminfd)
	{
		cache->ph = new AdminHandler(this, nfd, nid_);
	}
	else if (listenFd==clientfd)
	{
		cache->ph = new ClientHandler(this, nfd, nid_, cache);
	}
	else if (listenFd==policyfd)
	{
		cache->ph = new PolicyHandler(this, nfd);
	}
}

int GameNetHandler::connectFailed(int connectFd)
{
	if (isConnectSocket(connectFd)) 
	{
		bool bFind=false;
		for(int j=CST_Begin;j<CST_Cnt;j++)
		{
			for (size_t i = 0; i < m_ServerLst[j].size(); i++ )
			{
				if ( m_ServerLst[j][i].iFd == connectFd )
				{
					m_ServerLst[j][i].iFd = 0;
					m_ServerLst[j][i].Status = NET_DOWN ;
					bFind = true;
					//发送断线消息
					UpdateWorkingStatus::addEvent(eq, UpdateWorkingStatus_HG_Disconn, m_ServerLst[j][i].id);
				}
			}
			if(bFind)
			{
				switch(j)
				{
				case CST_World:
					{
						connectToWorld();
					}
					break;
// 				case CST_Star:
// 					{
// 						connectToStarSrv();
// 					}
// 					break;
// 				case CST_Country:
// 					{
// 						connectToCountry();
// 					}
// 					break;
				case CST_Plat:
					{
						connectToPlatSrv();
					}
					break;
// 				case CST_Fight:
// 					{
// 						connectToFightSrv();
// 					}
// 					break;
// 				case CST_Region:
// 					{
// 						connectToRegion();
// 					}
// 					break;
// 				case CST_Transfer:
// 					{
// 						connectTransferSrv();
// 					}
// 					break;
// 				case CST_Monitor:
// 					{
// 						connectToMonitor();
// 					}
// 					break;
// 				case CST_Rank:
// 					{
// 						connectToRankSrv();
// 					}
// 					break;
// 				case CST_Stat:
// 					{
// 						connectToStatSrv();
// 					}
//					break;
				}
				return 0;
			}
		}
	}
    return 0;
}

bool GameNetHandler::preNetEvent(time_t now)
{
	kickIdleClients(now);
	keepAliveWithWordAndPKSrv(now);

	lockjoin();
	if(mapJoinFd.size() > 0)
	{
		map<int, int64>::iterator iter = mapJoinFd.begin();
		while(iter != mapJoinFd.end())
		{
			NetCache* cache = getCacheFromFd(iter->first);
			if(cache != NULL)
			{
				cache->uid = iter->second;
			}
			++iter;
		}
		mapJoinFd.clear();
	}
	unlockjoin();

	lockkick();
	if (mapKickFd.size() > 0)
	{
		map<int, int64>::iterator iter = mapKickFd.begin();
		while(iter != mapKickFd.end())
		{
			NetCache* cache = getCacheFromFd(iter->first);
			if(cache != NULL && cache->uid == iter->second)
			{
				doCloseConnection(iter->first);
			}
			++iter;
		}
		mapKickFd.clear();
	}
	unlockkick();
	return true;
}

void GameNetHandler::kickIdleClients(time_t now)
{
	if ((now-last_timeout_check)>(1000*30))
	{
		last_timeout_check = now;
		map<int,NetCache*>::const_iterator it = fdCache.begin();
		vector<int> idleList;
		idleList.clear();
		for (it=fdCache.begin(); it!=fdCache.end(); it++)
		{
			NetCache *cache = it->second;
			if (cache!=NULL && cache->ph!=NULL
				&& cache->ph->handlerType()==ProtocolHandler::CLIENT)
			{
				if (cache->idle)
				{
					idleList.push_back(cache->fd);
					int64 uid = cache->uid;
					if (uid>0)
					{
						cache->ph->leave(uid);
					}
				}
				else
				{
					cache->idle = true;
				}
			}
		}
		if (idleList.size()>0)
		{
			for (size_t i=0; i<idleList.size(); i++)
			{
				doCloseConnection(idleList[i]);
			}
			LOG4CXX_INFO(logger_, idleList.size()<<" idle clients kicked out.");
		}
	}
}

bool GameNetHandler::connectToPlatSrv()
{
	if ( m_ServerLst[CST_Plat].size() == 0 )
	{
		for ( int i = 0; i < serverConfig.plat_num(); i++)
		{
			NetSockState worldd;
			memset( &worldd, 0, sizeof ( worldd ) );
			worldd.Status = NET_DOWN;
			m_ServerLst[CST_Plat].push_back( worldd );
		}
	}

	for (size_t i = 0; i < m_ServerLst[CST_Plat].size(); i++)
	{
		time_t  tNow = time( NULL );
		if ( (m_ServerLst[CST_Plat][i].Status == NET_DOWN ||
			m_ServerLst[CST_Plat][i].Status == NET_CONNECTING )
			&& tNow - m_ServerLst[CST_Plat][i].tLastTry > 5 )
		{
			m_ServerLst[CST_Plat][i].tLastTry = tNow;
			int iID = serverConfig.platID( i );
			m_ServerLst[CST_Plat][i].id = iID;
			struct addrinfo sa;
			LOG4CXX_DEBUG(logger_, "Connecting to plat server, ID=" << iID);
			int fd = createConnectSocket("platfd", serverConfig.plat_server_address(iID),
				toString(serverConfig.plat_gamed_Port(iID)), sa);
			if (fd < 0)
			{
				LOG4CXX_ERROR(logger_, "Connect to plat server failed! ID=" << iID);
				continue;
			}

			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
				DEFAULT_SERVER_SIZE);
			cache->ph = new PlatHandler(eq, fd, this);
			m_ServerLst[CST_Plat][i].iFd = fd;
			m_ServerLst[CST_Plat][i].Status = NET_CONNECTING;

#ifdef _WIN32
			connectSuccess(fd);
#endif
		}
	}

	return true;
}

// bool GameNetHandler::connectToCountry()
// {
// 	if ( m_ServerLst[CST_Country].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.country_num(); i++)
// 		{
// 			NetSockState worldd;
// 			memset( &worldd, 0, sizeof ( worldd ) );
// 			worldd.Status = NET_DOWN;
// 			m_ServerLst[CST_Country].push_back( worldd );
// 		}
// 	}
// 
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Country].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Country][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Country][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Country][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Country][i].tLastTry = tNow;
// 			int iID = serverConfig.countryID( i );
// 			m_ServerLst[CST_Country][i].id = iID;
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to pksrv server, ID=" << iID);
// 			int fd = createConnectSocket("pkfd", serverConfig.country_server_address(iID),
// 				toString(serverConfig.country_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to pksrv server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			cache->ph = new CountryHandler(eq, fd, this);
// 			m_ServerLst[CST_Country][i].iFd = fd;
// 			m_ServerLst[CST_Country][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectTransferSrv()
// {
// 	if ( m_ServerLst[CST_Transfer].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.transfer_num(); i++)
// 		{
// 			NetSockState worldd;
// 			memset( &worldd, 0, sizeof ( worldd ) );
// 			worldd.Status = NET_DOWN;
// 			m_ServerLst[CST_Transfer].push_back( worldd );
// 		}
// 	}
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Transfer].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Transfer][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Transfer][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Transfer][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Transfer][i].tLastTry = tNow;
// 			int iID = serverConfig.starID( i );
// 			m_ServerLst[CST_Transfer][i].id = iID;
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to pksrv server, ID=" << iID);
// 			int fd = createConnectSocket("pkfd", serverConfig.transfer_server_address(iID),
// 				toString(serverConfig.transfer_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to pksrv server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			//cache->ph = new TransferHandler(eq, fd, this);
// 			m_ServerLst[CST_Transfer][i].iFd = fd;
// 			m_ServerLst[CST_Transfer][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectToRegion()
// {
// 	if ( m_ServerLst[CST_Region].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.regionsrv_num(); i++)
// 		{
// 			NetSockState regiond;
// 			memset( &regiond, 0, sizeof ( regiond ) );
// 			regiond.Status = NET_DOWN;
// 			m_ServerLst[CST_Region].push_back( regiond );
// 		}
// 	}
// 
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Region].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Region][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Region][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Region][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Region][i].tLastTry = tNow;
// 			int iID = serverConfig.regionsrvID( i );
// 			m_ServerLst[CST_Region][i].id = iID;
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to region server, ID=" << iID);
// 			int fd = createConnectSocket("regiond", serverConfig.region_server_address(iID),
// 				toString(serverConfig.region_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to region server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			//cache->ph = new RegionHandler(eq, fd, this);
// 			m_ServerLst[CST_Region][i].iFd = fd;
// 			m_ServerLst[CST_Region][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// bool GameNetHandler::connectToFightSrv()
// {
// 	if ( m_ServerLst[CST_Fight].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.fightd_num(); i++)
// 		{
// 			NetSockState fightd;
// 			memset( &fightd, 0, sizeof ( fightd ) );
// 			fightd.Status = NET_DOWN;
// 			m_ServerLst[CST_Fight].push_back( fightd );
// 		}
// 	}
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Fight].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Fight][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Fight][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Fight][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Fight][i].tLastTry = tNow;
// 			int iID = serverConfig.fightdID( i );
// 			m_ServerLst[CST_Fight][i].id = iID;
// 			struct addrinfo sa;
// 
// 			LOG4CXX_DEBUG(logger_, "Connecting to fight server, ID=" << iID);
// 
// 			int fd = createConnectSocket("fightfd", serverConfig.fightd_server_address(iID),toString(serverConfig.fightd_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to fight server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 
// 			//cache->ph = new FightHandler(eq, fd, this);
// 			m_ServerLst[CST_Fight][i].iFd = fd;
// 			m_ServerLst[CST_Fight][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectToStarSrv()
// {
// 	if ( m_ServerLst[CST_Star].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.star_num(); i++)
// 		{
// 			NetSockState worldd;
// 			memset( &worldd, 0, sizeof ( worldd ) );
// 			worldd.Status = NET_DOWN;
// 			m_ServerLst[CST_Star].push_back( worldd );
// 		}
// 	}
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Star].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Star][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Star][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Star][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Star][i].tLastTry = tNow;
// 			int iID = serverConfig.starID( i );
// 			m_ServerLst[CST_Star][i].id = iID;
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to pksrv server, ID=" << iID);
// 			int fd = createConnectSocket("pkfd", serverConfig.star_server_address(iID),
// 				toString(serverConfig.star_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to pksrv server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			//cache->ph = new StarHandler(eq, fd, this);
// 			m_ServerLst[CST_Star][i].iFd = fd;
// 			m_ServerLst[CST_Star][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectToMonitor()
// {
// 	if( m_ServerLst[CST_Monitor].size() == 0 )
// 	{
// 		for(int i = 0; i<GameSrvCfg::Instance().monitor_num(); i++)
// 		{
// 			NetSockState worldd; 
// 			memset(&worldd,0, sizeof(worldd)); 
// 			worldd.Status = NET_DOWN; 
// 			m_ServerLst[CST_Monitor].push_back( worldd ); 
// 		}
// 	}
// 
// 	for(int i = 0; i<m_ServerLst[CST_Monitor].size(); i++)
// 	{
// 		time_t  tNow = time(NULL); 
// 		if((m_ServerLst[CST_Monitor][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Monitor][i].Status == NET_CONNECTING)
// 			&& tNow - m_ServerLst[CST_Monitor][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Monitor][i].tLastTry = tNow;
// 			int id = GameSrvCfg::Instance().monitorID(i); 
// 			m_ServerLst[CST_Monitor][i].id = id; 
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to monitor server, ID="<<id);
// 			int fd = createConnectSocket("msfd",GameSrvCfg::Instance().monitor_server_address(id),
// 				toString(GameSrvCfg::Instance().monitor_gamed_Port(id)), sa);
// 			if (fd<0) 
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to monitor server failed! ID="<<id);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd,*(sockaddr_in*)sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			//cache->ph = new MonitorHandler(eq, fd, this);
// 			m_ServerLst[CST_Monitor][i].iFd = fd;
// 			m_ServerLst[CST_Monitor][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectToRankSrv()
// {
// 	if ( m_ServerLst[CST_Rank].size() == 0 )
// 	{
// 		for ( int i = 0; i < serverConfig.ranksrv_num(); i++)
// 		{
// 			NetSockState worldd;
// 			memset( &worldd, 0, sizeof ( worldd ) );
// 			worldd.Status = NET_DOWN;
// 			m_ServerLst[CST_Rank].push_back( worldd );
// 		}
// 	}
// 
// 	for (size_t i = 0; i < m_ServerLst[CST_Rank].size(); i++)
// 	{
// 		time_t  tNow = time( NULL );
// 		if ( (m_ServerLst[CST_Rank][i].Status == NET_DOWN ||
// 			m_ServerLst[CST_Rank][i].Status == NET_CONNECTING )
// 			&& tNow - m_ServerLst[CST_Rank][i].tLastTry > 5 )
// 		{
// 			m_ServerLst[CST_Rank][i].tLastTry = tNow;
// 			int iID = serverConfig.starID( i );
// 			m_ServerLst[CST_Rank][i].id = iID;
// 			struct addrinfo sa;
// 			LOG4CXX_DEBUG(logger_, "Connecting to ranksrv server, ID=" << iID);
// 			int fd = createConnectSocket("rkfd", serverConfig.rank_server_address(iID),
// 				toString(serverConfig.rank_gamed_Port(iID)), sa);
// 			if (fd < 0)
// 			{
// 				LOG4CXX_ERROR(logger_, "Connect to rksrv server failed! ID=" << iID);
// 				continue;
// 			}
// 
// 			NetCache *cache = addConnection(fd, *(sockaddr_in*) sa.ai_addr,
// 				DEFAULT_SERVER_SIZE);
// 			//cache->ph = new RankHandler(eq, fd, this);
// 			m_ServerLst[CST_Rank][i].iFd = fd;
// 			m_ServerLst[CST_Rank][i].Status = NET_CONNECTING;
// 
// #ifdef _WIN32
// 			connectSuccess(fd);
// #endif
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool GameNetHandler::connectToStatSrv()
// {
// 	return true;
// 
// }

bool GameNetHandler::connectToWorld( ) {
	if( m_ServerLst[CST_World].size() == 0 )
	{
		for(int i = 0; i<serverConfig.worlddNum(); i++)
		{
			NetSockState worldd; 
			memset(&worldd,0, sizeof(worldd)); 
			worldd.Status = NET_DOWN; 
			m_ServerLst[CST_World].push_back( worldd ); 
		}
	}

	for(int i = 0; i<serverConfig.worlddNum(); i++)
	{
		time_t  tNow = time(NULL); 
		if((m_ServerLst[CST_World][i].Status == NET_DOWN ||
			m_ServerLst[CST_World][i].Status == NET_CONNECTING)
			 && tNow - m_ServerLst[CST_World][i].tLastTry > 5 )
		{
			m_ServerLst[CST_World][i].tLastTry = tNow;
			int id = serverConfig.worlddId(i); 
			m_ServerLst[CST_World][i].id = id; 
			struct addrinfo sa;
			LOG4CXX_DEBUG(logger_, "Connecting to world server, ID="<<id);
			int fd = createConnectSocket("wsfd",serverConfig.worlddIp(id),
				toString(serverConfig.worlddGamedPort(id)), sa);
			if (fd<0) 
			{
				LOG4CXX_ERROR(logger_, "Connect to world server failed! ID="<<id);
				continue;
			}

			NetCache *cache = addConnection(fd,*(sockaddr_in*)sa.ai_addr,
				DEFAULT_SERVER_SIZE);
			cache->ph = new WorldHandler(eq, fd, this);
			m_ServerLst[CST_World][i].iFd = fd;
			m_ServerLst[CST_World][i].Status = NET_CONNECTING;

#ifdef _WIN32
			connectSuccess(fd);
#endif
		}
	}
	
	return true;
}

void GameNetHandler::keepAliveWithWordAndPKSrv(time_t now)
{	
	if ((now-last_keepalive_check)>10000)
	{
		connectToWorld(); // reconnect
		//connectToStarSrv();
		connectToPlatSrv();
		//connectToFightSrv();
		//connectTransferSrv();
		//connectToCountry();
		//connectToRegion();
		//connectToMonitor();
		//connectToRankSrv();
		//connectToStatSrv();

		last_keepalive_check = now;
		for(int j=CST_Begin;j<CST_Cnt;j++)
		{
			for (size_t i = 0; i < m_ServerLst[j].size(); i++)
			{
				if ( m_ServerLst[j][i].Status != NET_DOWN && m_ServerLst[j][i].Status != NET_CONNECTING )
				{
					sendIntSizedFdString( m_ServerLst[j][i].iFd, "ka");
				}
			}
		}
    }
}

bool GameNetHandler::sendSizedTypeFdString(int fd, int type, const string& str,bool bUseRc4)
{
	NetCache *cache = getCacheFromFd(fd);
	if (cache == NULL)
	{
		return false;
	}
	//if(str.length() > _MAX_RC4BUFF_SIZE)
	if(str.length() > 204800)
	{
		LOG4CXX_ERROR(logger_, "String Send Client is To Large!!! size:"<<str.length()<<",type:"<<type);
		return false;
	}

	int index = cache->getNextIndex();
	index = htonl(index);
	unsigned short utype = (unsigned short) type;
	utype = htons(type);
	unsigned short ulen = (unsigned short) str.length() + sizeof (unsigned short)
		+ sizeof (unsigned int) ;
	ulen = htons(ulen);

	if(bUseRc4)
	{
		memset(m_charrc4,0,_MAX_RC4BUFF_SIZE);
		cache->m_rc4Send.Process((const unsigned char *)str.c_str(),(unsigned char *)m_charrc4,str.length());
	}

// 	int nLeng = str.length();
// 	int nLengx = strlen(m_charrc4);
// 
// 	if(nLeng != nLengx)
// 	{
// 		LOG4CXX_ERROR(logger_,"length is not same "<<nLeng<<" "<<nLengx);
// 	}

	lockSend(); // we use this to lock the send procedure
	sendFdString(fd, (const char*) (&ulen), sizeof (unsigned short) );
	sendFdString(fd, (const char*) (&utype), sizeof (unsigned short) );
	sendFdString(fd, (const char*) (&index), sizeof (unsigned int) );

	bool succ = false;
	//转换
	if(bUseRc4)
	{
		succ = sendFdString(fd, m_charrc4, str.length());
	}
	else
	{
		succ = sendFdString(fd, str.c_str(), str.length());
	}
	
	unlockSend();
	return succ;
}

bool GameNetHandler::SetRc4Keys(int fd,string& strSend,string& strReceive)
{
	NetCache *cache = getCacheFromFd(fd);
	if (cache == NULL)
	{
		return false;
	}	

	cache->m_rc4Send.Setup((const unsigned char *)strSend.c_str(),strSend.length(),true);
	cache->m_rc4Receive.Setup((const unsigned char *)strReceive.c_str(),strReceive.length(),false);

	memset(m_charrc4,0,_MAX_RC4BUFF_SIZE);

	return true;
}

int GameNetHandler::initSockets()
{
	while (!connectToWorld())
	{
		usleep(3000);
	}
	clientfd = createListenSocket("clientfd", toString(serverConfig.gamedServerPort(nid_)),
		serverConfig.gamedBindIp(nid_));
	if (clientfd<0)
	{
		return -1;
	}
	adminfd = createListenSocket("adminfd", toString(serverConfig.gamedAdminPort(nid_)),
		serverConfig.gamedBindIp(nid_));
	if (adminfd<0)
	{
		return -1;
	}
	policyfd = createListenSocket("policyfd", toString(serverConfig.gamedPolicyPort(nid_)),
		serverConfig.gamedBindIp(nid_));
	if (policyfd<0)
	{
		return -1;
	}

	last_timeout_check = 0;
	last_keepalive_check = 0;

#ifdef _WIN32
	return max(clientfd, max(adminfd, policyfd));
#else
	return 0;
#endif
}
