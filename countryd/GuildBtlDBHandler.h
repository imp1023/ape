#pragma once
#include <map>
#include <list>
#include <pthread.h>
#include "../common/const_def.h"

#include <log4cxx/logger.h>
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#include <windows.h>
#else
#include <tcrdb.h>
#endif
#include "countryDB.pb.h"

using namespace std;
class CountryEventHandler;
class User;


#define THREADNUM 8



class GuildBtlDBHandler;
class Country_FightSendData;
class GuidlBtlRetWay;
class GuildBtlRet;

enum GuildBtlDBType
{
	GuildBtlDB_USER = 0,	//工会战阵容
	GuildBtlDB_RESULT,		//结果

	GuildGvgDB_USER	= 10,	//GVG阵容
	GuildGvgDB_RESULT,		//GVG结果
};	


class MsgGuildBtlDBPut
{
public:
	MsgGuildBtlDBPut()
	{
		m_nUserID = 0;
	}
	~MsgGuildBtlDBPut()
	{}
	int64		m_nUserID;
	string		m_strKey;
	string		m_strInfo;
};

class MsgGuildBtlDBGet
{
public:
	MsgGuildBtlDBGet()
	{
		m_nRegion	= 0;
		m_nGuild1	= 0;
		m_nGuild2	= 0;
		m_nWay		= 0;
		m_nTurns	= 0;
		m_nGetUserID= 0;
		m_nGameID	= 0;
		m_nCityID	= 0;
		m_nMaxPlayerNum = 0;
	}
	~MsgGuildBtlDBGet()
	{}
	enum{
		e_maxplayer_eachmsg = 5
	};
	int m_nRegion;
	int m_nGuild1;
	int m_nGuild2;
	int m_nWay;
	int m_nTurns;
	int m_nGameID;
	GuildBtlDBType m_emType;
	vector<int64> m_lstGuildUser1;
	vector<int64> m_lstGuildUser2;

	int64	m_nGetUserID;	//查看战报玩家ID

	int	m_nCityID;
	int m_nMaxPlayerNum;
};

struct GuildBtlDBThread
{
	GuildBtlDBHandler* pHandler;// 处理函数类
	int              no;      // 线程号
};

class Event;
class CGuild;
class GuildBtlDBHandler
{
public:
	GuildBtlDBHandler(void);
	~GuildBtlDBHandler(void);

	void	initThread();
	void	termThread();
	void	Run(int no);

	//push
	bool	SafePushGuildBtlUpdate(int64 nUserID,GuildBtlDBType emType,const DB_C_Guild_FightSendData* pDBGuildBtl);
	bool	SafePushGuildBtlResult(GuidlBtlRetWay* pRetWay,int nGuild1,int nGuild2,int nRegion,int nTurns,int nWay);

	bool	SafePushGuildGvgUpdate(int64 nUserID,GuildBtlDBType emType,const DB_C_Gvg_FightSendData* pDBGuildBtl);
	bool	SafePushGuildGvgResult(GuildBtlRet* pRet,int nGuild1,int nGuild2,int nRegion,int nCityID,int nSession);
	
	//get
	bool	DealGuildBtl(CGuild* pGuild1,CGuild* pGuild2,int nRegion,int nTurns,int nWay);
	bool	SafeLoadGuildBtlResult(int64 nLoadUserID,int nGuild,int nRegion,int nTurns,int nGameID);

	bool	DealGvgBtl(CGuild* pGuild1,CGuild* pGuild2,int nRegion,int nSession,int nWay,int nCityID);
	bool	SafeLoadGvgBtlResult(int64 nLoadUserID,int nCityID,int nRegion,int nSession,int nGameID);

	void	SetEventHandler(CountryEventHandler* eh);
	CountryEventHandler* GetEventHandler(void);

	static GuildBtlDBHandler& Instance()
	{
		static GuildBtlDBHandler inst;
		return inst;
	}
protected:
	static void* StarInfoProc(void* arg);

	bool	GetDBKey(int64 nUserID,GuildBtlDBType emType,string& strKey);
	bool	GetDBKey(int nRegion,int nGuildID,int nSession,int nWay,string& strKey);
	bool	GetGvgDBKey(int nRegion,int nCity,int nSession,string& strKey);
	void	Run4Load(int no,vector<MsgGuildBtlDBGet*>& vecMessageGet);
	void	Run4Save(int no,vector<MsgGuildBtlDBPut*>& vecMessagePut);

	Event*	AddEventRequest(int64 nUserID,GuildBtlDBType emType);

	bool	LoadGuildBtlInfo(int64 nUserID,string strKey, string& strInfo);
	bool	SaveGuildBtlInfo(int64 nUserID,string strKey, string& strInfo);

//	bool	LoadGvgBtlInfo(int64 nUserID,string strKey, string& strInfo);
//	bool	SaveGvgBtlInfo(int64 nUserID,string strKey, string& strInfo);

	int     GetDBGuildBtlId(int64 uid);
	TCRDB*	GetDBGuildBtl(int nDbStarId);

// 	int     GetDBGvgBtlId(int64 uid);
// 	TCRDB*	GetDBGvgBtl(int nDbStarId);

	void	LockMessageGet(int no);
	void	UnLockMessageGet(int no);	

	void	LockMessagePut(int no);
	void	UnLockMessagePut(int no);	

	time_t	GetStartTime(){return m_ltStart;}

	void FillCountryFightSendData(DB_C_Guild_FightSendData& dbFight,Country_FightSendData* pFightData);
	void FillCountryFightSendData(DB_C_Gvg_FightSendData& dbFight,Country_FightSendData* pFightData);

protected:
	time_t				m_ltStart;	//启动时间

	CountryEventHandler*	m_pEventHandler;
	bool				m_bEnable;

	map<int,void *>		m_mapDBGuildBtl;

	list<MsgGuildBtlDBGet*>	m_listMessageGet[THREADNUM];
	list<MsgGuildBtlDBPut*>	m_listMessagePut[THREADNUM];

	pthread_mutex_t		m_mutexStarMsgGet[THREADNUM];
	pthread_mutex_t		m_mutexStarMsgPut[THREADNUM];

	pthread_t			m_threadStarInfo[THREADNUM];
	bool				m_bRunning;

	GuildBtlDBThread      m_ThredPara[THREADNUM];

	log4cxx::LoggerPtr	logger_;
};

//////////////////inline////////////////////
inline
void GuildBtlDBHandler::SetEventHandler(CountryEventHandler* pEventHandler)
{
	m_pEventHandler = pEventHandler;
}

inline
CountryEventHandler*	GuildBtlDBHandler::GetEventHandler(void)
{
	return m_pEventHandler;
}

//inline
//void GuildBtlDBHandler::SetEnable(bool bEnable)
//{
//	m_bEnable = bEnable;
//}

inline 
void GuildBtlDBHandler::LockMessageGet(int no)
{
	if (no<0 || no>=THREADNUM)
		return;
	pthread_mutex_lock(&m_mutexStarMsgGet[no]);
}

inline
void GuildBtlDBHandler::UnLockMessageGet(int no)
{
	if (no<0 || no>=THREADNUM)
		return;
	pthread_mutex_unlock(&m_mutexStarMsgGet[no]);
}

inline 
void GuildBtlDBHandler::LockMessagePut(int no)
{
	if (no<0 || no>=THREADNUM)
		return;
	pthread_mutex_lock(&m_mutexStarMsgPut[no]);
}

inline
void GuildBtlDBHandler::UnLockMessagePut(int no)
{
	if (no<0 || no>=THREADNUM)
		return;
	pthread_mutex_unlock(&m_mutexStarMsgPut[no]);
}
