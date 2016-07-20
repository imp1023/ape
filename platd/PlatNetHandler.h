#pragma once

#include <log4cxx/logger.h>
#include <map>
#include <string>
#include <pthread.h>
#include "../event/EventQueue.h"
#include "../net/NetCache.h"
#include "../net/NetHandler.h"
#include "../common/const_def.h"
#include "../common/ServerConfig.h"

using namespace std;

extern ServerConfig serverConfig;

class EventQueue;

class PlatNetHandler : public NetHandler 
{
public:
	static const int MAX_EVENTS = 256;
	static const int MAX_QUEUE_LENGTH = 32;
	EventQueue *eq_;

	PlatNetHandler(EventQueue *eq,int nid);
	virtual ~PlatNetHandler() {};

	bool sendEventToGamed(Event *e, int gameid);
	bool sendEventToAllGamed(Event *e);
	bool sendEvnetToOneGamed(Event *e);

	int getGamedFdFromGamedId(int gameid);
	bool registerConnection(int fd, int id);
	int64 getIdFromFd(int fd);
	int getFdFromId(int id);
	void removeConnection(int fd);

protected:
	int initSockets();
	inline bool isListenSocket(int fd);
	inline bool isConnectSocket(int) {return false;}
	inline int readCacheSize(int listenFd);
	void createProtocolHandler(NetCache *cache, int listenFd);
	int connectFailed(int) {return 0;}
	bool isConnecting( int  ) { return false; }; 
	bool connectSuccess( int fd ){ return false; }; 
private:
	int nid_;   // node_id
	int gamedfd;
	map<int, int> idFd_;			  // gamed id to fd
	map<int64, long long> secrets;
	time_t last_keepalive_check; // keep alive with world

	bool preNetEvent(time_t now);
	void keepAliveWithGame(time_t now);
};
