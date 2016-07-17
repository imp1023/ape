#ifndef _GAME_CACHE_MODULE_H_
#define _GAME_CACHE_MODULE_H_
#pragma once

#include <list>
#include <vector>
#include <map>
using namespace std;
#include <log4cxx/logger.h>
#include "../common/const_def.h"
#include "../common/db_util.h"
#include "../common/Singleton.h"

class GameEventHandler;
class User;

class GameCacheModule
{
public:
    GameCacheModule(void);
    ~GameCacheModule(void);

    void    SetEnable(bool bEnable);
    void    WriteError(string strError);
    bool    ReadError();

    bool    GetUserInfo(int64 uid, string& strUserInfo);
    bool    PutUserInfo(int64 uid, string& strUserInfo);
    bool    RemoveUserInfo(int64 uid);
    bool    HasSaveUser(int64 uid);

    void    AddRemoveUser(int64 uid);
    void    AddSaveUser(int64 uid);

    bool    CanRemoveUser(int64 uid);
    bool    UpdateRemoveUser();

    bool    UpdateSaveUser();


    TCRDB*  GetDBCache();
    bool    CanUse();
    void    SetEventHandler(GameEventHandler* eh);
    GameEventHandler* GetEventHandler(void);

protected:
    GameEventHandler* m_pEventHandler;
    bool            m_bEnable;

    void *          m_pDbCache;
    time_t          m_timeUpdataRemove;
    time_t          m_timeUpdateSave;

    map<int64,bool> m_mapUserSave;
    map<int64,bool> m_mapUserRemove;

    log4cxx::LoggerPtr  m_logger;
};

//////////////////inline////////////////////
inline
void GameCacheModule::SetEventHandler(GameEventHandler* pEventHandler)
{
    m_pEventHandler = pEventHandler;
}

inline
GameEventHandler* GameCacheModule::GetEventHandler(void)
{
    return m_pEventHandler;
}

typedef Singleton<GameCacheModule> GameCacheInst;

#endif

