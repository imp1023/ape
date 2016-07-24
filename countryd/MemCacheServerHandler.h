
#pragma once

#include "../common/statistics.h"
#include "../common/counter.h"
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <pthread.h>
#include "../common/const_def.h"
#include "../common/distribution.h"

#include <log4cxx/logger.h>
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#else
#include <tcrdb.h>
#endif

class CountryEventHandler;
class CCity;
class CCountry;
class DB_C_UserList;
class DB_C_GuildList;
class DB_C_ResourceData;
class CGvgCity;
class DB_C_WarGameData;
class DB_C_BossBtlMsg;
class MemCacheServerHandler
{
    typedef MemCacheServerHandler     MSH_T;
    typedef std::map<int, std::string>  IPList;
    typedef std::map<int, int>          PORTList;
    typedef std::map<int, void*>        ConnList;
public:
    MemCacheServerHandler();
    ~MemCacheServerHandler();
public:
    static MSH_T*       GetInst();
public:
	static bool         GetMemDBInfo(const string& strKey, std::string& dist);
    static bool         SaveMemDBInfo(string strKey,std::string& dist);

	static bool			SafePushMemDB(int nKey,CCity* pCity,time_t revision_);
	static bool			SafePushMemDB(int nKey,CCountry* pCountry,time_t revision_);
	static bool			SafePushMemDB(int nKey,DB_C_GuildList* pDBGuildLst,time_t revision_);
	static bool			SafePushMemDB(int nKey,DB_C_UserList* pDBUserLst,time_t revision_);
	static bool			SafePushMemDB(int nKey,DB_C_ResourceData* pDBResourceData,time_t revision_);
	static bool			SafePushMemDB(int nKey,CGvgCity* pCity,time_t revision_);
    static bool         SafePushMemDB(int nKey,DB_C_WarGameData *pDBWarGame,time_t revision_);
	static bool         SafePushMemDB(int nKey,DB_C_BossBtlMsg *pDBBossBtlMsg,time_t revision_);

	static bool			SaveAllMemDBData(int64 revision,bool urgent = false);
	static bool			SaveAllCityMemDBData(int64 revision,bool urgent = false);
	static bool			SaveAllCountryMemDBData(int64 revision,bool urgent = false);
	static bool			SaveAllGuildLstMemDBData(int64 revision,bool urgent = false);
	static bool			SaveAllUserLstMemDBData(int64 revision,bool urgent = false);
	static bool			SaveAllResourceDBData(int64 revision,bool urgent = false);
	static bool			SaveAllGvgCityMemDBData(int64 revision,bool urgent = false);
    static bool         SaveAllWarGameMemDBData(int64 revision,bool urgent = false);
	static bool         SaveAllBossBtlMsg(int64 revision,bool urgent = false);
public:
	void				acquireLock() {pthread_mutex_lock(&mutex_);}
	void				releaseLock() {pthread_mutex_unlock(&mutex_);}

    int                 GetServerNumber();
    int                 GetServerId(const string& platID);
    void                LoadConfig();
    TCRDB*              GetDB(int id);
    bool                CanUse();
    void                SetEnbale(bool enable);
//	void				SetEventHandler(CountryEventHandler* eh);
//	CountryEventHandler*		GetEventHandler(void){return m_eh;}
	map<int,CCity*>&			GetCityList(void){return m_CityList;}
	map<int,CCountry*>&			GetCountryList(void){return m_CountryList;}
	map<int,DB_C_GuildList*>&	GetGuildList(void){return m_GuildList;}
	map<int,DB_C_UserList*>&	GetUserList(void){return m_UserList;}
	map<int,DB_C_ResourceData*>&	GetResourceList(void){return m_ResourceList;}
	map<int,CGvgCity*>&			GetGvgCityList(void){return m_GvgCityList;}
    map<int,DB_C_WarGameData*>& GetWarGameList(void){return m_WarGameList;}
	map<int,DB_C_BossBtlMsg*>&	GetBossBtlMsgList(void){return m_BossBtlMsgList;}
	

	time_t				GetSaveUpdateTime(){return m_SaveUpdate;}
	void				SetSaveUpdateTime(time_t save_time){m_SaveUpdate = save_time;}
	//void				SetCountryRevision(const string& platID,time_t revision_){m_mapRevision[platID.c_str()] = revision_;}
	//map<string, int64>& GetCountryRevision(){return m_mapRevision;}
	/*Statistics &		GetStatistics();
	Counter &			GetCounter();*/
private:
	pthread_mutex_t		mutex_;

	static MSH_T*       g_pInst;
    int                 m_nServerNum;
	map<int,CCity*>		m_CityList;
	map<int,CCountry*>	m_CountryList;
	map<int,DB_C_UserList*>	m_UserList;
	map<int,DB_C_GuildList*> m_GuildList;
	map<int,DB_C_ResourceData*> m_ResourceList;
	map<int,CGvgCity*>	m_GvgCityList;
    map<int,DB_C_WarGameData*> m_WarGameList;
	map<int,DB_C_BossBtlMsg*> m_BossBtlMsgList;

	//map<string, int64>  m_mapRevision;
    IPList              m_xIpList;
    PORTList            m_xProtList;
    ConnList            m_xConnList;
    bool                m_bEnabled;
    bool                m_bInited;
	static log4cxx::LoggerPtr logger;
	//CountryEventHandler*	m_eh;
	time_t				m_SaveUpdate;

	
} ;
///////////////////////////////////////

inline bool MemCacheServerHandler::CanUse()
{
    return m_bEnabled && m_bInited && m_nServerNum!=0;
}


