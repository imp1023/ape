
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

class PlatEventHandler;
class DB_Plat;
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
	static bool         GetPlatInfo(const string& platID, std::string& dist);
    static bool         UpdatePlatInfo(const string& platID,std::string& dist);

	static bool			SafePushPlat(const string& platID,DB_Plat * pPlat,time_t revision_);
	static bool			SaveAllPlatData(int64 revision,bool urgent = false);
public:
	void				acquireLock() {pthread_mutex_lock(&mutex_);}
	void				releaseLock() {pthread_mutex_unlock(&mutex_);}

    int                 GetServerNumber();
    int                 GetServerId(const string& platID);
    void                LoadConfig();
    TCRDB*              GetDB(int id);
    bool                CanUse();
    void                SetEnbale(bool enable);
	void				SetEventHandler(PlatEventHandler* eh);
	PlatEventHandler*	GetEventHandler(void){return m_eh;}
	map<string,DB_Plat*>&	GetPlatList(void){return m_PlatList;}
	time_t				GetSaveUpdateTime(){return m_SaveUpdate;}
	void				SetSaveUpdateTime(time_t save_time){m_SaveUpdate = save_time;}
	//void				SetPlatRevision(const string& platID,time_t revision_){m_mapRevision[platID.c_str()] = revision_;}
	//map<string, int64>& GetPlatRevision(){return m_mapRevision;}
	/*Statistics &		GetStatistics();
	Counter &			GetCounter();*/
private:
	pthread_mutex_t		mutex_;

	static MSH_T*       g_pInst;
    int                 m_nServerNum;
	map<string,DB_Plat*>	m_PlatList;
	//map<string, int64>  m_mapRevision;
    IPList              m_xIpList;
    PORTList            m_xProtList;
    ConnList            m_xConnList;
    bool                m_bEnabled;
    bool                m_bInited;
	static log4cxx::LoggerPtr logger;
	PlatEventHandler*	m_eh;
	time_t				m_SaveUpdate;

	
} ;
///////////////////////////////////////

inline bool MemCacheServerHandler::CanUse()
{
    return m_bEnabled && m_bInited && m_nServerNum!=0 && m_eh != 0;
}


