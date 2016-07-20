#pragma once

#include <log4cxx/logger.h>
#ifdef _WIN32
#define MAINTENANCE_LOG_LEVEL_ERR 3
#define MAINTENANCE_MONITOR_MODULE_INTERFACE_LOG_FORMAT ""
//class TMsgLog {
//public:
//	TMsgLog (unsigned int levelvalue, const char* operID) {}
//	~TMsgLog (void) {}
//	//通过UDP上报的msg，传入参数hashkey为计算上报server的依据
//	int netprintf (unsigned long msgid, unsigned int hashkey, const char* fmt, ...) {return 0;}
//	int netprintf (unsigned long msgid, unsigned int hashkey, const char* fmt, char* ap[]) {return 0;}
//	//原有日志API
//	int msgprintf (unsigned int level, const char* fmt, ...) {return 0;}
//	int msgprintf (unsigned int level, time_t timestamp, const char* fmt, ...) {return 0;}
//	int msgprintf (unsigned int level, unsigned long msgid, const char* fmt, ...) {return 0;}
//	int msgprintf (unsigned int level, unsigned long msgid, time_t timestamp, const char* fmt, ...) {return 0;}
//};
#endif

class User;

enum MSG2QQType
{
	MQ_Regist		= 100,
	MQ_Logon		= 101,
	MQ_Logout		= 102,
	MQ_Back2Home	= 103,
	MQ_VisitFrd		= 104,
	MQ_EnterHall	= 105,
	MQ_LeaveHall	= 106,
	MQ_EnterRoom	= 107,
	MQ_LeaveRoom	= 108,
	MQ_EndBattle	= 109,
	MQ_BuyProp		= 110,
	MQ_SellProp		= 111,
	MQ_WebBuy		= 112,
	MQ_Strengthen	= 113,
	MQ_Forge		= 114,
	MQ_Compose		= 115,
	MQ_WaterTree	= 116,
	MQ_PickBox		= 117,
	MQ_HarvestTree	= 118,
};

class CMsg2QQ
{
public:
	CMsg2QQ();
	~CMsg2QQ();
	static CMsg2QQ* GetInstance();

	void TellMsg(MSG2QQType emType,User* pUser,int nGoldChg,int nExpChg,unsigned int uFrdPlatformID);	
//	void TellMsg(MSG2QQType emType,User* pUser,int nGoldChg,int nExpChg,unsigned int uFrdPlatformID);
private:
	log4cxx::LoggerPtr logger_;
//	TMsgLog m_msg;
};
class CMsg2QQDB
{
public:
	CMsg2QQDB();
	~CMsg2QQDB();
	static CMsg2QQDB* GetInstance();

	void TellSaveDB(int nUseTime,const char* szIP1,const char* szIP2, int nPort2);
	void TellLoadDB(int nUseTime,const char* szIP1,const char* szIP2, int nPort2);
private:
	log4cxx::LoggerPtr logger_;
//	TMsgLog m_msg;
};
