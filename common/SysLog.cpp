#include "SysLog.h"
#include "distribution.h"
#include <time.h>
#include "string-util.h"
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

//#include <log4cxx/logger.h>
#endif




const char  SZLOG_SYS[]				= "LogSys";	

CSysLog::CSysLog(void)
{
	m_nSrvID		= 0;
	m_pLogSys		= NULL;	
	m_lastCheck		= 0;
	m_bShowLog		= false;
	m_nSendLogLv	= 0;
	m_nModul		= 0;
	m_nModulVal		= 0;
	m_bInUse		= false;
	m_nPos			= 0;
#ifdef _WIN32
	InitializeCriticalSection(&m_Section); 
#else
	pthread_mutex_init(&m_mutex,0);	
#endif
}

CSysLog::~CSysLog(void)
{
#ifdef WIN32
	::DeleteCriticalSection(&m_Section);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
	if(m_pLogSys)
	{
		m_pLogSys->close();
		delete m_pLogSys;	
	}
}
CSysLog* CSysLog::GetInstance()
{
	static CSysLog log;
	return &log;
}

void CSysLog::Quit()
{
	m_SysLogNet.Quit();
}
void CSysLog::SetLogInfo(bool bGameLog,int nSrvID,
						 string strLogDir,string strLogName,
						 string strAddr,string strPort,
						 bool bShowLog,int nSendLogLv,
						 int nModul,int nModulVal)
{
	m_bGameLog	= bGameLog;
	m_bShowLog	= bShowLog;
	m_nSendLogLv = nSendLogLv;
	m_nModul	= nModul;
	m_nModulVal	= nModulVal;

	m_nSrvID	= nSrvID;
	m_strLogDir = strLogDir;
	m_strLogName= strLogName;
	if(m_bShowLog||m_nSendLogLv>0)
		m_bInUse = true;
	if(m_nSendLogLv>0)
	{
		m_SysLogNet.StartThread(strAddr,strPort);
	}
}
bool CSysLog::CreateDir(const char* szDir)
{
	if(!TURNON_LOG)
		return true;
	if(!m_bShowLog)
		return true;
	char szCurDir[512];
	memset(szCurDir,0,512);

#ifdef _WIN32
	GetCurrentDirectory(512,szCurDir);
	if(!SetCurrentDirectory(szDir))
	{
		if(!CreateDirectory(szDir,NULL))
		{
			cout << "Can't Create Directory: " << szDir << endl;
			SetCurrentDirectory(szCurDir);
			return 0;
		}
	}
	SetCurrentDirectory(szCurDir);
#else
	getcwd(szCurDir,512);
	if(chdir(szDir)!=0)
	{
		char szCmd[512];
		memset(szCmd,0,512);
		sprintf(szCmd,"mkdir %s -p \r\n",szDir);
		system(szCmd);
		//if(open(szDir,O_RDWR|O_CREAT|O_DIRECTORY)!=0)
		//{
		//	cout << "Create Dir:"<<szCurDir<<"Failed!"<<endl;
			//LOG4CXX_FATAL(logger_, "Create Dir:"+szCurDir+"Failed!");
		//	return 0;
		//}
	}
	chdir(szCurDir);
#endif


	return true;
}
bool CSysLog::CreateLog()
{
	if(!TURNON_LOG)
		return true;
	if(!m_bShowLog)
		return true;
	time_t lt = time(NULL);
	if(lt-m_lastCheck<300)
		return true;
	m_lastCheck = lt;
	struct tm newtime;
	char szTime[128];
	localtime_r(&lt, &newtime);
	strftime(szTime, 128-1, "%Y-%m-%d", &newtime);

	m_strLogDay = szTime;
	char szFile[512];


#ifdef _WIN32
	char szDir[512];
	memset(szDir,0,512);
	GetCurrentDirectory(512,szDir);
	if(!CreateDir(SZLOG_SYS))
		return false;
	SetCurrentDirectory(szDir);
	sprintf(szFile,"%s\\%s\\%s%d_%s.log",szDir,SZLOG_SYS,m_strLogName.c_str(),m_nSrvID,m_strLogDay.c_str());

#else
	if(!CreateDir(m_strLogDir.c_str()))
		return false;
	//strcpy(szDir,"/data/fish/lin/Test1/Test1/LogSys");
	sprintf(szFile,"%s/%s%d_%s.log",m_strLogDir.c_str(),m_strLogName.c_str(),m_nSrvID,m_strLogDay.c_str());
#endif

	m_pLogSys		= new ofstream(szFile, ios::out | ios::app);
	if(m_pLogSys==NULL)
	{
		//LOG4CXX_FATAL(logger_, "Create File:"+szFile+"Failed!");
		cout << "Create File:"<<szFile<<"Failed!"<<endl;
		return false;
	}

#ifdef _WIN32
	SetCurrentDirectory(szDir);
#else

#endif	
	return true;
}
void CSysLog::WriteCurTime()
{
	struct tm newtime;
	char szTime[128];

	time_t lt = time(NULL);
	localtime_r(&lt, &newtime);
	strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", &newtime);
	*this << szTime;
}

void CSysLog::ChgLogFile()
{
	if(!TURNON_LOG)
		return ;
	if(!m_bShowLog)
		return;
	struct tm newtime;
	char szTime[128];

	time_t lt = time(NULL);
	localtime_r(&lt,&newtime);
	strftime(szTime, 128-1, "%Y-%m-%d", &newtime);
	if(m_strLogDay.compare(szTime)==0)
		return;
	//m_strLogDay = szTime;
	LogLock();
	if(m_pLogSys)
	{
		m_pLogSys->close();
		delete m_pLogSys;
		m_pLogSys		= NULL;	
	}
	CreateLog();
	LogUnLock();
}

void CSysLog::BeginMsg()
{
	memset(m_szBuf,0,MAX_LOG_SMG_LENGTH);
	m_nPos=0;
}

void CSysLog::EndMsg()
{
	m_szBuf[m_nPos] = 0;
	if(m_pLogSys&&m_bShowLog)
		*m_pLogSys << m_szBuf << std::endl;
}

void CSysLog::SendMsg()
{
	m_SysLogNet.PushSendStr(m_szBuf);
}

bool CSysLog::IsStatUser(int64 nUserID)
{
	if(!m_bGameLog)
		return false;
	if(m_nModul==0)
		return true;
	return (nUserID%m_nModul)==m_nModulVal;
}

bool CSysLog::InUse()
{
	return m_bInUse;
}

CSysLog& CSysLog::operator << (const int& value)
{
	sprintf(m_szBuf+m_nPos,"%d,",value);
	m_nPos = strlen(m_szBuf);
	return *this;
}

CSysLog& CSysLog::operator << (const char& value)
{
	sprintf(m_szBuf+m_nPos,"%c,",value);
	m_nPos = strlen(m_szBuf);
	return *this;
}

CSysLog& CSysLog::operator << (const float& value)
{
	sprintf(m_szBuf+m_nPos,"%f,",value);
	m_nPos = strlen(m_szBuf);
	return *this;	
}

CSysLog& CSysLog::operator << (const double& value)
{
	sprintf(m_szBuf+m_nPos,"%lf,",value);
	m_nPos = strlen(m_szBuf);
	return *this;	
}

CSysLog& CSysLog::operator << (const time_t& value)
{
	sprintf(m_szBuf+m_nPos,"%s,",toString(value).c_str());
	m_nPos = strlen(m_szBuf);
	return *this;
}
#ifndef _WIN32
CSysLog& CSysLog::operator << (const int64& value)
{
	sprintf(m_szBuf+m_nPos,"%s,",toString(value).c_str());
	m_nPos = strlen(m_szBuf);
	return *this;
}
#endif
CSysLog& CSysLog::operator << (char* szBuf)
{
	return *this << (const char*)szBuf;
}
CSysLog& CSysLog::operator << (const char* szBuf)
{
	sprintf(m_szBuf+m_nPos,"%s,",szBuf);
	m_nPos = strlen(m_szBuf);
	return *this;
}
CSysLog& CSysLog::operator << (string& str)
{		
	sprintf(m_szBuf+m_nPos,"%s,",str.c_str());
	m_nPos = strlen(m_szBuf);
	return *this;
}
CSysLog& CSysLog::operator << (const string& str)
{
	sprintf(m_szBuf+m_nPos,"%s,",str.c_str());
	m_nPos = strlen(m_szBuf);

	return *this;
}
