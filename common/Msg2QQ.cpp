#include <time.h>
#include <math.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

#include "Msg2QQ.h"
#include "../logic/User.h"

const int QQ_LOGID		= 1000000017;
const int QQ_APPID		= 12309;
const int QQ_VERSION		= 1;
const int QQ_SOURCE_XIAOYOU	= 2;
const int QQ_SOURCE_KONGJIAN    = 1;
const int QQ_STATE		= 0;

/*
log日志ID：
2025000021 

模块ID:
梦幻海底用户数据读取   216000028 
梦幻海底用户数据写入   216000029
梦幻海底用户数据库      216000031

接口ID：
读取梦幻海底用户数据  116418067  
写入梦幻海底用户数据  116418068 

string ip1 = “172.30.10.12”;  //主调模块所在server的IP

string ip2 = “172.30.10.15”; //被调模块所在server的IP

int res = 0;                    //调用返回值

int use_time = 327.478;       //调用延时
*/
const long unsigned int QQ_DB_LOGID		= 2025000021L;
const unsigned int QQ_DB_LOAD_MID	= 216000028; //被调模块ID
const unsigned int QQ_DB_SAVE_MID	= 216000029; //被调模块ID
const unsigned int QQ_DB_ID			= 216000031; //主调模块ID

const unsigned int QQ_DB_SAVE_FID	= 116418067; //被调模块接口ID
const unsigned int QQ_DB_LOAD_FID	= 116418068; //被调模块接口ID

CMsg2QQ::CMsg2QQ()//:m_msg(3,"app")
{
	logger_ = log4cxx::Logger::getLogger("Msg2QQ");
}

CMsg2QQ::~CMsg2QQ()
{
}

CMsg2QQ* CMsg2QQ::GetInstance()
{
	static CMsg2QQ log;
	return &log;
}

void CMsg2QQ::TellMsg(MSG2QQType emType,User* pUser,int nGoldChg,int nExpChg,unsigned int uFrdPlatformID)
{
	return;
	//if(pUser==NULL)
	//	return;
	////nGoldChg and nExpChg is unsigned int
	////nGoldChg = abs(nGoldChg);
	////nExpChg = abs(nExpChg);
	//unsigned int uiRequestUin = pUser->platform_id_2int();
	//unsigned int uiClientIP = inet_addr(pUser->GetLastLoginIp().c_str());
	//unsigned int uiOperTime = (unsigned int)time(NULL);
	//unsigned int uiSource = 2;
	//if(pUser->GetPlattype()==PLAT_XIAOYOU)
	//{	
	//	uiSource = QQ_SOURCE_XIAOYOU;
	//}
	//else if (pUser->GetPlattype()== PLAT_QZONE)
	//{
	//	uiSource = QQ_SOURCE_KONGJIAN;
	//}
	//else
	//{
	//	return;
	//}
	//int nRtn = m_msg.netprintf(QQ_LOGID,uiRequestUin,"%d%d%d%d%d%d%d%s%d%d%d%d%s",
	//		QQ_APPID,QQ_VERSION,uiSource,(int)emType,QQ_STATE,
	//		uiRequestUin,uFrdPlatformID,pUser->GetPlatformId().c_str(),
	//		uiClientIP,uiOperTime,
	//		nGoldChg,nExpChg,"");

	//char szInfo[1024];
	//sprintf(szInfo,"Rtn:%d,Send:%d,%d,%d,%d,%d,%u,%u,%s,%u,%u,%u,%u,%s\n",
	//	nRtn,QQ_APPID,QQ_VERSION,uiSource,(int)emType,QQ_STATE,
	//	uiRequestUin,uFrdPlatformID,pUser->GetPlatformId().c_str(),
	//	uiClientIP,uiOperTime,
	//	nGoldChg,nExpChg,"");
	//LOG4CXX_INFO(logger_, "Msg2QQ:"<<szInfo);
}


CMsg2QQDB::CMsg2QQDB()
//:m_msg(3,"app")
{
	logger_ = log4cxx::Logger::getLogger("Daemon");
}

CMsg2QQDB::~CMsg2QQDB()
{

}

CMsg2QQDB* CMsg2QQDB::GetInstance()
{
	static CMsg2QQDB log;
	return &log;
}

void CMsg2QQDB::TellSaveDB(int nUseTime,const char* szIP1,const char* szIP2, int nPort2)
{
	return;
	//int nRtn = m_msg.msgprintf(
	//	MAINTENANCE_LOG_LEVEL_ERR, QQ_DB_LOGID, time(NULL),
	//	MAINTENANCE_MONITOR_MODULE_INTERFACE_LOG_FORMAT,
	//	QQ_DB_ID, QQ_DB_SAVE_MID, QQ_DB_SAVE_FID,
	//	inet_addr(szIP1), inet_addr(szIP2), 0, nPort2,
	//	"GameDataSaver.cpp", 490, "2010-11-02 19:48:43",
	//	"write", 0, 0, nUseTime,
	//	-1, -1, -1, -1, -1, "", "", "", "", ""
	//	);
	//char szInfo[1024];
	//sprintf(szInfo,"Rtn:%d,Send:%d,%d,%d,%s,%s,%d,%d\n",
	//	nRtn,QQ_DB_ID,QQ_DB_SAVE_MID,QQ_DB_SAVE_FID,szIP1,szIP2,nPort2,nUseTime);
	//LOG4CXX_INFO(logger_, "Msg2QQ:"<<szInfo);
}

void CMsg2QQDB::TellLoadDB(int nUseTime,const char* szIP1,const char* szIP2, int nPort2)
{
	return;
	//int nRtn = m_msg.msgprintf(
	//	MAINTENANCE_LOG_LEVEL_ERR, QQ_DB_LOGID, time(NULL),
	//	MAINTENANCE_MONITOR_MODULE_INTERFACE_LOG_FORMAT,
	//	QQ_DB_ID, QQ_DB_LOAD_MID, QQ_DB_LOAD_FID,
	//	inet_addr(szIP1), inet_addr(szIP2), 0, nPort2,
	//	"GameDataSaver.cpp", 527, "2010-11-02 19:48:43",
	//	"read", 0, 0, nUseTime,
	//	-1, -1, -1, -1, -1, "", "", "", "", ""
	//	);

	//char szInfo[1024];
	//sprintf(szInfo,"Rtn:%d,Send:%d,%d,%d,%s,%s,%d,%d\n",
	//	nRtn,QQ_DB_ID,QQ_DB_LOAD_MID,QQ_DB_LOAD_FID,szIP1,szIP2,nPort2,nUseTime);
	//LOG4CXX_INFO(logger_, "Msg2QQ:"<<szInfo);

}

