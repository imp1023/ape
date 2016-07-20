#ifndef _GAMENETHANDLER_H_
#define _GAMENETHANDLER_H_
#pragma once

#ifdef _WIN32
	#include <WinSock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif
#include <log4cxx/logger.h>
#include <pthread.h>
#include <map>
#include <string>
#include "../common/ServerConfig.h"
#include "../net/NetHandler.h"
#include "../common/const_def.h"

extern ServerConfig serverConfig;

using namespace std;

class GameEventHandler;
class EventQueue;

enum ConnectSrvType
{
	CST_Begin = 0,
	CST_World = CST_Begin,
	//CST_Star,
	//CST_Country,
	CST_Plat,
	//CST_Fight,
	//CST_Region,
	//CST_Transfer,
	//CST_Monitor,
	//CST_Rank,
	//CST_Stat,
	CST_Cnt,
};

enum NET_STATUS
{
	NET_DOWN =1,
	NET_INACTIVE=2,
	NET_ACTIVE =3,
	NET_CONNECTING =4 
};
struct NetSockState
{
	int id;				//·þÎñÆ÷ID
	int iFd;			//socketID
	NET_STATUS Status; 
	time_t tLastTry; 
};

class GameNetHandler : public NetHandler
{
public:
	EventQueue *eq;
	int nid() {return nid_;}

	GameNetHandler(EventQueue *eventQueue, int nid);
	virtual ~GameNetHandler()
	{
		pthread_mutex_destroy(&kick_mutex);
		pthread_mutex_destroy(&join_mutex);
	}
public:
	void addUser(int fd, int64 uid);
	void kickUser(int fd, int64 uid);
	//int64 getIdFromFd(int fd);
	//bool sendToWorld(int fd, const string& str);
	bool sendToWorld(int nWorldSrvID,const string& str);
	//bool sendToStar(int nStarSrvID,const string& str);
	//bool sendToCountry(int nCountrySrvID,const string& str);
	bool sendToPlat(int nPlatSrvID,const string& str);
	//bool sendToFight(int nFightSrvID,const string& str);
	//bool sendToTransfer(int nTransferID,const string& str);
	//bool sendToRegion(int nRegionSrvID,const string& str);
	//bool sendToRank(int nSrvID,const string& str);
	//bool sendToStat(int nSrvID,const string& str);
	//bool sendToMonitor(int nMonitorSrvID,const string& str);
	bool sendDataToClient(int fd, int type, const string& str,bool bUseRc4);

	virtual bool sendSizedTypeFdString(int fd, int type, const string& str,bool bUseRc4);
	virtual bool SetRc4Keys(int fd,string& strSend,string& strReceive);

private:
	bool sendToServer(int nSrvID,ConnectSrvType emType,const string& str);
	const char* GetSrvName(int emType);
protected:
	int initSockets();
	inline bool isListenSocket(int fd);
	inline bool isConnectSocket(int fd);
	inline int readCacheSize(int listenFd);
	void createProtocolHandler(NetCache *cache, int listenFd);
	int connectFailed(int connectFd);

	virtual bool isConnecting( int fd ); 
	virtual bool connectSuccess( int fd ); 
private:
	inline void lockkick() { pthread_mutex_lock(&kick_mutex); }
	inline void unlockkick() { pthread_mutex_unlock(&kick_mutex); }
	inline void lockjoin() { pthread_mutex_lock(&join_mutex); }
	inline void unlockjoin() { pthread_mutex_unlock(&join_mutex); }

private:
	static const int SECRET_TIMEOUT_SEC = 600;

	//log4cxx::LoggerPtr login_logger_;
	int nid_;
	//int wsfd; // world server fd
	//vector<NetSockState> WorldServer;
	//vector<NetSockState> StarServer;
	//vector<NetSockState> CountryServer;
	//vector<NetSockState> PlatServer;
	//vector<NetSockState> FightServer;
	//vector<NetSockState> TransferServer;
	//vector<NetSockState> RegionServer;
	//vector<NetSockState> MonitorServer;
	vector<NetSockState> m_ServerLst[CST_Cnt];

	int clientfd; // main clientfd for client
	int adminfd; // clientfd for admin telnets
	int policyfd; // policy file fd
	int rankfd;// rank server fd
	time_t last_timeout_check; // last timeout check time
	time_t last_keepalive_check; // keep alive with world

	map<int, int64>	mapKickFd;
	map<int, int64> mapJoinFd;
	pthread_mutex_t kick_mutex;
	pthread_mutex_t join_mutex;

	bool preNetEvent(time_t now);
	bool connectToWorld();
	bool connectToPlatSrv();
	bool connectToCountry();
// 	bool connectToStarSrv();
// 	bool connectToFightSrv();
// 	bool connectTransferSrv();
// 	bool connectToRegion();
// 	bool connectToMonitor();
// 	bool connectToRankSrv();
// 	bool connectToStatSrv();

	void kickIdleClients(time_t now);
	void keepAliveWithWordAndPKSrv(time_t now);

	char m_charrc4[_MAX_RC4BUFF_SIZE];
};



#endif
