#pragma once
#include <fstream>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <WinSock2.h>
#include "wtypes.h"
#include <Windows.h>
#endif

#include <pthread.h> 
#include "const_def.h"
#include "LogDef.h"
#include "SysLogNet.h"

const int	TURNON_LOG				= 1;			//是否写入日志	
const int	TURNON_DEBUG			= 1;			//是否写入debug日志
using namespace std;


const int MAX_LOG_SMG_LENGTH = 1024*5;
class CSysLog
{
public:

private:
	CSysLog(void);
	~CSysLog(void);
public:
	static CSysLog* GetInstance();
	void Quit();
	void SetLogInfo(bool bGameLog,int nSrvID,string strLogDir,string strLogName,string strAddr,string strPort,bool bShowLog,int nSendLogLv,int nModul,int nModulVal);
	bool CreateLog();
	bool CreateDir(const char* szDir);
	void ChgLogFile();

	void WriteCurTime();

	ofstream* GetSysLogOF(){return m_pLogSys;}

	bool IsStatUser(int64 nUserID);
	int	 GetSrvID() {return m_nSrvID;}
	void BeginMsg();
	void EndMsg();
	void SendMsg();	
	bool InUse();
#ifdef _WIN32
	inline void LogLock() {
		EnterCriticalSection( &m_Section ); 
	}
	inline void LogUnLock() {
		LeaveCriticalSection(&m_Section);
	}
#else
	inline void LogLock() {
		pthread_mutex_lock(&m_mutex);
	}
	inline void LogUnLock() {
		pthread_mutex_unlock(&m_mutex);
	}
#endif

	template <class T> 
	CSysLog& operator << (const T &value)
	{
		sprintf(m_szBuf+m_nPos,"%d,",value);
		m_nPos = strlen(m_szBuf);

		//if(m_pLogSys&&m_bShowLog)
		//	*m_pLogSys << value <<",";
		//if(m_bSendLog)
		//{
		//	sprintf(m_szBuf+m_nPos,"%d,",value);
		//	m_nPos = strlen(m_szBuf);
		//}
		return *this;
	}
	CSysLog& operator << (const int& value);
	CSysLog& operator << (const time_t& value);
#ifndef _WIN32
	CSysLog& operator << (const int64& value);
#endif
	CSysLog& operator << (const char& value);
	CSysLog& operator << (const double& value);
	CSysLog& operator << (const float& value);
	CSysLog& operator << (char* szBuf);
	CSysLog& operator << (const char* szBuf);
	CSysLog& operator << (string& str);
	CSysLog& operator << (const string& str);
private:
	
private:
	CSysLogNet	m_SysLogNet;
	bool		m_bShowLog;
	int			m_nSendLogLv;
	int			m_nModul;
	int			m_nModulVal;
	bool		m_bInUse;
	int			m_nSrvID;
	string		m_strLogDir;
	string		m_strLogName;
	bool		m_bGameLog;		//true:gamelog false:halllog

	string		m_strLogDay;
	ofstream*	m_pLogSys;	
	time_t		m_lastCheck;
	int			m_nPos;
	char		m_szBuf[MAX_LOG_SMG_LENGTH];
#ifdef _WIN32
	CRITICAL_SECTION m_Section; 
#else
	pthread_mutex_t m_mutex;
#endif

};

extern const char* log_string(int log_type);
extern bool log_sampling(int log_type);

template<class T1, class V1, class V2, class V3, class V4, class V5, class V6, class V7>
void SYS_STAT(int64 uid, int logType, int level, string openid, int totalBuy, string regDays, int credit, int region, int channel, int vipLevel,
              T1 t1, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7)
{
    CSysLog* pLog = CSysLog::GetInstance();
    if (!pLog->InUse())
        return;

    pLog->LogLock();
    pLog->BeginMsg();
    pLog->WriteCurTime();
    *pLog << pLog->GetSrvID() << uid << log_string(logType) << level << openid << totalBuy << regDays << credit << region << channel << vipLevel
          << t1 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
    pLog->EndMsg();
	if (log_sampling(logType) && !pLog->IsStatUser(uid))
	{
		pLog->SendMsg();
	}
    pLog->LogUnLock();
}

template<class T1, class V1, class V2, class V3, class V4, class V5, class V6, class V7>
void SYS_GLOBAL_STAT(int logType, T1 t1, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7)
{
	CSysLog* pLog = CSysLog::GetInstance();
	if (!pLog->InUse())
		return;

	pLog->LogLock();
	pLog->BeginMsg();
	pLog->WriteCurTime();
	*pLog << pLog->GetSrvID() << "" <<log_string(logType) << t1 << v1 << v2 << v3 << v4 << v5 << v6 << v7
		<<"" << "" << "" << "" << "" << "" << "" << "";
	pLog->EndMsg();
	if (log_sampling(logType))
	{
		pLog->SendMsg();
	}
	pLog->LogUnLock();
}

