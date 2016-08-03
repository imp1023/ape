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
#include "../common/const_def.h"

#include <map>
#include <list>
#include <string>

#include <curl/curl.h>
#include "event.pb.h"

using namespace std;

#define HTTPTHREADNUM 4

class GameEventHandler;


class HttpReq
{
public:
	HttpReq();
	~HttpReq();

	void setUrl(const string& url);

	void addHeader(const string& key, const string& val);
	void clearHeader();

	int perform();

	string getRequireUrl(int nrand,const string& szopenid,int64 nuid,int nRegion,int ntype,int ntypesec,const string& szargvs);

    string getRequireUrl(const string& szopenid,int64 nuid, int nTime, int nIP, int nReportType, int nDomain, int nModifyfee, unsigned int nSrvIP);

	string getTestUrl(int nrand);

	string&	getContent(){return content_;}

private:
	CURL*           curl_;
	string			content_;
	curl_slist*     header_;

	static const string szkGetUrl;
	static const string szkOpenID;
	static const string szkUid;
	static const string szkRegion;
	static const string szkType;
	static const string szkTypeSec;
	static const string szkArgvs;
};

class HttpRequireHandler;

struct HttpThred
{
	HttpRequireHandler* pHandler;// 处理函数类
	int              no;      // 线程号
};

class Event;
class User;
class HttpRequireHandler
{
public:
	HttpRequireHandler(void);
	~HttpRequireHandler(void);

	void	initThread();
	void	Run(int no);

	bool	SafePushHttpRequest(string szplatid,int64 nuid,int nregion,int ntype,int ntypesec,string szargvs="");

    bool    SafePushHttpLogOnToQQ(User *pUser, int nDomain);

    bool    SafePushHttpLogOutToQQ(User *pUser, int nDomain);

    bool    SafePushHttpAddCreditToQQ(User *pUser, int nDomain, int nModifyfee);

    bool    SafePushHttpWebBuyToQQ(User *pUser, int nDomain, int nModifyfee);

    bool    SafePushHttpSelfRegistToQQ(User *pUser, int nDomain);

    bool    SafePushHttpInviteToQQ(User *pUser, int nDomain);

    bool    SafePushHttpAcceptToQQ(User *pUser, int nDomain);

    bool    SafePushHttpOnLineToQQ(User *pUser, int nDomain);

	bool	CanUse();
	void	SetEnable(bool bEnable);

	void	SetEventHandler(GameEventHandler* eh);
	GameEventHandler* GetEventHandler(void);

	static HttpRequireHandler& Instance()
	{
		static HttpRequireHandler inst;
		return inst;
	}

protected:
	static void* GameHttpProc(void* arg);

	Event*	AddHttpRequest(int64 uid,HttpAnswerMsg& rAnswerMgr);

	int     GetHttpId(int64 uid);

	void	LockMessage(int no);
	void	UnLockMessage(int no);

	time_t	GetStartTime(){return m_ltStart;}
protected:
	log4cxx::LoggerPtr logger_;

	time_t				m_ltStart;	//启动时间

	GameEventHandler*	m_pEventHandler;
	bool				m_bEnable;

	list<HttpAskMsg*>	m_listMessage[HTTPTHREADNUM];

	pthread_mutex_t		m_mutexHttpMsg[HTTPTHREADNUM];

	pthread_t			m_threadHttpInfo[HTTPTHREADNUM];
	bool				m_bRunning;

	HttpThred			m_ThredPara[HTTPTHREADNUM];

	//HttpReq				m_HttpReq[HTTPTHREADNUM];
};

//////////////////inline////////////////////
inline void HttpRequireHandler::SetEventHandler(GameEventHandler* pEventHandler)
{
	m_pEventHandler = pEventHandler;
}

inline GameEventHandler*	HttpRequireHandler::GetEventHandler(void)
{
	return m_pEventHandler;
}

inline void HttpRequireHandler::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

inline void HttpRequireHandler::LockMessage(int no)
{
	if (no<0 || no>=HTTPTHREADNUM)
		return;
	pthread_mutex_lock(&m_mutexHttpMsg[no]);
}

inline void HttpRequireHandler::UnLockMessage(int no)
{
	if (no<0 || no>=HTTPTHREADNUM)
		return;
	pthread_mutex_unlock(&m_mutexHttpMsg[no]);
}
