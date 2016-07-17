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
#include <list>
#include "../net/NetHandler.h"
#include "const_def.h"

using namespace std;

class Event;
class EventQueue;

class CSysLogNet : public NetHandler
{
public:
	enum SLogStat_STATUS
	{
		SLSTAT_DOWN =1,
		SLSTAT_INACTIVE=2,
		SLSTAT_ACTIVE =3,
		SLSTAT_CONNECTING =4 
	};
	struct SLogStatItem
	{
		int			iFd; 
		SLogStat_STATUS Status; 
		time_t		tLastTry; 
		string		strAddr;
		string		strPort;
		time_t		m_tmLastSend; 
	};
public:
	CSysLogNet(void);
	virtual ~CSysLogNet(void);
public:
	bool StartThread(string strAddr,string strPort);
	static void* ThreadProc(void* lParam);
	void Run();
	void Quit();

	void PushSendStr(const char* szBuf);
	bool PopSendStr(string& str);

protected:
	int initSockets();
	bool connectToStatSrv();
	inline int readCacheSize(int listenFd);
	bool isConnecting( int fd ); 
	int  connectFailed(int connectFd);
	bool connectSuccess( int fd ); 
	void createProtocolHandler(NetCache *cache, int listenFd);
	bool isListenSocket(int fd){ return false;}
	bool isConnectSocket(int fd) {return true;}

private:
	SLogStatItem  m_LogStat;
private:
	log4cxx::LoggerPtr login_logger_;

	time_t last_keepalive_check; // keep alive with world

	bool preNetEvent(time_t now);
	void keepAlive(time_t now);
public:
#ifdef _WIN32
	inline void LogLock() {
		EnterCriticalSection( &m_Section ); 
	}
	inline void LogUnLock() {
		LeaveCriticalSection(&m_Section);
	}
#else
	inline void LogLock() {
		pthread_mutex_lock(&m_mutexNet);
	}
	inline void LogUnLock() {
		pthread_mutex_unlock(&m_mutexNet);
	}
#endif


private://send msg args
	list<string> m_lstStr2Send;
	bool	m_bRunnig;
private://socket args
	log4cxx::LoggerPtr logger_;
	pthread_t	m_thSysLogNet;
private:
#ifdef _WIN32
	CRITICAL_SECTION m_Section; 
#else
	pthread_mutex_t m_mutexNet;
#endif
};
