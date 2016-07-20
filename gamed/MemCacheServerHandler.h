/* 
 * File:   MemCacheServerHandler.h
 * Author: Kidd
 * 用于处理好友缓存的类
 * Created on 2011年12月23日, 上午10:32
 */

#ifndef MEMCACHESERVERHANDLER_H
#define	MEMCACHESERVERHANDLER_H

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

class GameEventHandler;
class User;
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
	static bool         GetUserInfo(const int64 &uid, std::string& dist);//,std::string& npcstring,std::string& effstring);
    static bool         UpdateUserInfo(const int64 &uid,std::string& dist);//,std::string& npcstring,std::string& effstring);
	static bool			RemoveUserInfo(const int64 &uid);
	static bool			UpdateRemoveUser();
	static bool			SafePushUser(const int64 &uid,User * pUser);
	static bool			SafePushRemoveList(const int64& uid);
	static bool			SaveAllUserData(int64 revision,bool urgent = false);
public:
	void				acquireLock() {pthread_mutex_lock(&mutex_);}
	void				releaseLock() {pthread_mutex_unlock(&mutex_);}

    int                 GetServerNumber();
    int                 GetServerId(const int64 &uid);
    void                LoadConfig();
    TCRDB*              GetDB(int id);
    bool                CanUse();
    void                SetEnbale(bool enable);
	void				SetEventHandler(GameEventHandler* eh);
	GameEventHandler*	GetEventHandler(void){return m_eh;}
	map<int64,User*>&	GetUserList(void){return m_UserList;}
	map<int64,bool>&	GetRemoveList(){return m_RemoveList;}
	time_t				GetRemoveUpdateTime(){return m_RemoveUpdate;}
	void				SetRemoveUpdateTime(time_t update_time){m_RemoveUpdate = update_time;}
	time_t				GetSaveUpdateTime(){return m_SaveUpdate;}
	void				SetSaveUpdateTime(time_t save_time){m_SaveUpdate = save_time;}
	/*Statistics &		GetStatistics();
	Counter &			GetCounter();*/
private:
	pthread_mutex_t		mutex_;

	static MSH_T*       g_pInst;
    int                 m_nServerNum;
	map<int64,User*>	m_UserList;
    IPList              m_xIpList;
    PORTList            m_xProtList;
    ConnList            m_xConnList;
    bool                m_bEnabled;
    bool                m_bInited;
	static log4cxx::LoggerPtr logger;
	GameEventHandler*	m_eh;
	time_t				m_RemoveUpdate;
	time_t				m_SaveUpdate;
	map<int64,bool>		m_RemoveList;
	
} ;
///////////////////////////////////////

inline bool MemCacheServerHandler::CanUse()
{
    return m_bEnabled && m_bInited && m_nServerNum!=0 && m_eh != 0;
}
#endif	/* MEMCACHESERVERHANDLER_H */

