#define _CRT_SECURE_NO_WARNINGS
#include "../event/DataHandler.h"
#include "GameDataHandler.h"
#include "GameDataSaver.h"
#include "../logic/dbinterface.pb.h"
#include "../common/Clock.h"
#include "../logic/GameServerConfig.h"
#include "../common/distribution.h"
#include "../common/SysLog.h"
#include "../logic/User.h"
#include "../logic/Player.h"
#include "GameCacheModule.h"
#include "Daemon.h"

using namespace std;

GameDataHandler::GameDataHandler(int nid) : DataHandler(nid)
{
    nid_ = nid;
    m_pDataSaver = new GameDataSaver(nid);
    m_mapUsers.clear();
    revision_ = 1;
    logger_ = log4cxx::Logger::getLogger("GameDataHandler");

    next_user_id_ = 1;
    next_item_id_ = 1;

    loadAllocateSetting();
	LoadGlobalStarInfo();

    m_mapPlatId2Uid.clear();
    pthread_mutex_init(&m_mutexData, NULL);
    timeUpdate = time(NULL);
    m_timeRemoveUpdate = Clock::getCurrentSystemTime();

}

GameDataHandler::~GameDataHandler(void)
{
    hash_map<int64, User*>::iterator iter;
    for (iter = m_mapUsers.begin(); iter != m_mapUsers.end(); ++iter)
    {
        delete iter->second;
    }

	map<int64, User*>::iterator iter1;
    acquireDataLock();
    for (iter1 = m_mapLoadedUser.begin(); iter1 != m_mapLoadedUser.end(); ++iter1)
    {
        delete iter1->second;
    }
    releaseDataLock();

    delete m_pDataSaver;

	pthread_mutex_destroy(&m_mutexData);

	m_listRemove.clear();
}

void GameDataHandler::loadAllocateSetting()
{
    m_pDataSaver->loadAllocateSetting(next_user_id_, next_item_id_);
    int step = 1000;
    if (next_user_id_ > 1)
    {
        next_user_id_ += step;
    }
    if (next_item_id_ > 1)
    {
        next_item_id_ += step;
    }
    m_pDataSaver->saveAllocateSetting(next_user_id_, next_item_id_);
}

void GameDataHandler::LoadGlobalStarInfo()
{
	m_pDataSaver->LoadGlobalStarInfo();
}

void GameDataHandler::SaveGlobalStarInfo()
{
	m_pDataSaver->SaveGlobalStarInfo();
}

void GameDataHandler::saveAllocateSetting()
{
    m_pDataSaver->saveAllocateSetting(next_user_id_, next_item_id_);
}

int64 GameDataHandler::allocateItemID(int num)
{
    int64 iid = next_item_id_;
    next_item_id_ += num;
    iid |= ((int64) (nid_ & 0xffff)) << 48;
    return iid;
}

void GameDataHandler::mapUidToUser(int64& uid, User* user)
{
    hash_map<int64, User*>::iterator iter = m_mapUsers.find(uid);
    if (iter != m_mapUsers.end())
    {
        if (user != NULL)
        {
            if (iter->second == NULL)
            {
                m_mapUsers[uid] = user;
            }
            else
            {
                delete user;
				user = NULL;
            }
        }
    }
    else
    {
        m_mapUsers.insert(make_pair(uid, user));
    }

	m_listRemove.push_back(uid);

    map<int64, LoadStatus>::iterator load_iter = m_mapLoadList.find(uid);
    if (load_iter != m_mapLoadList.end())
    {
        m_mapLoadList.erase(load_iter);
    }
}

void GameDataHandler::mapPlatidToUid(const string& pid, int64& uid)
{
    hash_map<string, int64>::iterator iter = m_mapPlatId2Uid.find(pid);
    if (iter != m_mapPlatId2Uid.end())
    {
        m_mapPlatId2Uid[pid] = uid;
    }
    else
    {
        m_mapPlatId2Uid.insert(make_pair(pid, uid));
    }
}

void GameDataHandler::init()
{
    GameCacheInst::instance().SetEnable(true);
}

void GameDataHandler::tick()
{
    updateUserMap();
    updatePlatidMap();
    saveAllUserData();
	UpdateRemoveUser();
	GameCacheInst::instance().UpdateSaveUser();
}

void GameDataHandler::quit()
{
	saveAllUserData(true);
}

void GameDataHandler::termThreads()
{
	m_pDataSaver->termThreads();
}

void GameDataHandler::UpdateRemoveUser()
{
	if (GameServerConfig::Instance().FreeEnableFlag() == 0)
	{
		return;
	}

	if ((int) m_mapUsers.size() <  GameServerConfig::Instance().FreeMaxUserCnt())
	{
		return;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if (timeNow - m_timeRemoveUpdate < GameServerConfig::Instance().FreeUpdateTime())
	{
		return;
	}

	vector<User*> vecRemoveUser;
	list<int64>::iterator iter = m_listRemove.begin();
	int nFreeCnt = 0;
	while (iter != m_listRemove.end())
	{
		int64 uid = *iter;
		hash_map<int64, User*>::iterator iter_user = m_mapUsers.find(uid);
		if (iter_user == m_mapUsers.end() )
		{//error
			iter++;
			continue;
		}

		User * pUser = iter_user->second;
		if (pUser == NULL)
		{
			//iter++;
			iter = m_listRemove.erase(iter);
			continue;
		}

		if (pUser->Online() == true)
		{
			iter++;
			continue;
		}

		if (timeNow < pUser->revision() + GameServerConfig::Instance().FreeMaxUserTime() *1000/*3600 * 1000 * 24*/)
		{
			iter++;
			continue;
		}

		map<int64, User*>::iterator iter_dirty = m_mapDirtyUsers.find(pUser->uid());
		if (iter_dirty != m_mapDirtyUsers.end())
		{
			iter++;
			continue;
		}

		nFreeCnt++;
		RemoveUser(pUser);
		iter = m_listRemove.erase(iter);

		if (nFreeCnt >= GameServerConfig::Instance().FreeCnt())
		{
			break;
		}

		time_t time_last = Clock::getCurrentSystemTime();
		if (time_last - timeNow > 50)
		{
			break;
		}
	}

	m_timeRemoveUpdate = Clock::getCurrentSystemTime();
}

void GameDataHandler::RemoveUser(User *pUser)
{
	if (pUser)
	{
		hash_map<int64, User*>::iterator iter;
		iter = m_mapUsers.find(pUser->uid());
		if (iter != m_mapUsers.end())
		{
			m_mapUsers.erase(iter);
			delete pUser;
			pUser = NULL;
		}
	}	
}

void GameDataHandler::RemovePlatidMap(const string& pid)
{
	hash_map<string, int64>::iterator iter = m_mapPlatId2Uid.find(pid);
	if (iter != m_mapPlatId2Uid.end())
	{
		m_mapPlatId2Uid.erase(pid);
	}
}

void GameDataHandler::RemovePlatidFromDb(const string& pid)
{
	m_pDataSaver->RemovePlatidFromDb(pid);
}

void GameDataHandler::RemoveUserFromDb(int64 uid)
{
	m_pDataSaver->RemoveUserFromDb(uid);
}

void GameDataHandler::updateUserMap()
{
    acquireDataLock();
    map<int64, User*>::iterator iter = m_mapLoadedUser.begin();
    while (iter != m_mapLoadedUser.end())
    {
        map<int64, User*>::iterator oiter = iter;
        ++iter;
        int64 uid = oiter->first;
        User* user = oiter->second;
        m_mapLoadedUser.erase(oiter);

        if (user != NULL)
        {
            const string& pid = user->openid();
            mapPlatidToUid(pid, uid);
        }
        LOG4CXX_DEBUG(logger_, "Update user map uid=" << uid << " ,user=" << user);
        mapUidToUser(uid, user);
    }
    releaseDataLock();
}

void GameDataHandler::updatePlatidMap()
{
    acquireDataLock();
    map<string, int64>::iterator iter = m_mapLoadedPlatId.begin();
    while (iter != m_mapLoadedPlatId.end())
    {
        map<string, int64>::iterator oiter = iter;
        ++iter;
        string pid = oiter->first;
        int64 uid = oiter->second;
        m_mapLoadedPlatId.erase(oiter);

        LOG4CXX_DEBUG(logger_, "Update user map platid=" << pid << " ,uid=" << uid);
        mapPlatidToUid(pid, uid);
    }
    releaseDataLock();
}

void GameDataHandler::updateLoadUser(int64& uid, User* user)
{
    acquireDataLock();
    map<int64, User*>::iterator iter = m_mapLoadedUser.find(uid);
    if (iter != m_mapLoadedUser.end())
    {
        if (user != NULL)
        {
            if (iter->second == NULL)
            {
                m_mapLoadedUser[uid] = user;
            }
            else
            {
                delete user;
				user = NULL;
            }
        }
    }
    else
    {
        m_mapLoadedUser.insert(make_pair(uid, user));
    }
    releaseDataLock();
}

void GameDataHandler::updateLoadPlatid(const string& pid, int64& uid)
{
    acquireDataLock();
    map<string, int64>::iterator iter = m_mapLoadedPlatId.find(pid);
    if (iter != m_mapLoadedPlatId.end())
    {
        m_mapLoadedPlatId[pid] = uid;
    }
    else
    {
        m_mapLoadedPlatId.insert(make_pair(pid, uid));
    }
    releaseDataLock();
}

void GameDataHandler::markUserDirty(User* user)
{
    if (user == NULL)
    {
        return;
    }

	if (!GameCacheInst::instance().HasSaveUser(user->uid()))
	{
		user->setMemRevision(revision_);
		GameCacheInst::instance().AddSaveUser(user->uid());
	}

    int64 uid = user->uid();
    map<int64, User*>::iterator iter = m_mapDirtyUsers.find(uid);
    if (iter == m_mapDirtyUsers.end())
    {
        user->setRevision(revision_);
        m_mapDirtyUsers.insert(make_pair(uid, user));
        LOG4CXX_DEBUG(logger_, "Marking user dirty: uid=" << uid << ", revision" << revision_);
    }
}

void GameDataHandler::saveAllUserData(bool urgent)
{
    if (!urgent && revision_ < timeUpdate + 10 * 1000)
    {
        return;
    }
    m_pDataSaver->safeSaveAllUser(revision_, this, urgent);
    saveAllocateSetting();
    timeUpdate = revision_;
}

void GameDataHandler::saveUserData(User* user)
{
    m_pDataSaver->safeSaveUser(this, user);
}

void GameDataHandler::loadUserData(int64& uid)
{
	if (LoadUserFromMemCache(uid))
		return;
    m_pDataSaver->safeLoadUser(this, uid);
}

void GameDataHandler::savePlatidMap(const string& pid, int64& uid)
{
    m_pDataSaver->safeSaveMap(this, pid, uid);
}

void GameDataHandler::loadPlatidMap(const string& pid)
{
    m_pDataSaver->safeLoadMap(this, pid);
}

User* GameDataHandler::getUser(int64 uid, LoadStatus* status, bool load)
{
    LoadStatus tmp_status, *status_ = (status == NULL) ? &tmp_status : status;
    if (!isUidLocal(uid))
    {
        *status_ = LOAD_MISS;
        return NULL;
    }

    hash_map<int64, User*>::iterator iter = m_mapUsers.find(uid);
    if (iter != m_mapUsers.end())
    {
        if (iter->second == NULL)
        {
            *status_ = LOAD_EMPTY;
            return NULL;
        }
        *status_ = LOAD_SUCCESS;
        return iter->second;
    }

    if (!load)
    {
        *status_ = LOAD_EMPTY;
        return NULL;
    }

    map<int64, LoadStatus>::iterator load_iter = m_mapLoadList.find(uid);
    if (load_iter != m_mapLoadList.end())
    {
        *status_ = LOAD_WAITING;
        return NULL;
    }

    // max wait for 10000
    int load_count = m_mapLoadList.size();
    if (load_count >= MAX_LOAD_WAITING)
    {
        *status_ = LOAD_BUSY;
        return NULL;
    }

    m_mapLoadList.insert(make_pair(uid, LOAD_WAITING));
    *status_ = LOAD_WAITING;
    loadUserData(uid);
    return NULL;
}

bool GameDataHandler::isPlatidLocal(const string& platid)
{
    return ServerConfig::Instance().gameid(distribution::getRegion(platid)) == nid_;
}

bool GameDataHandler::isUidLocal(int64 uid)
{
    return ServerConfig::Instance().gameid(distribution::getRegion(uid)) == nid_;
}

LoadStatus GameDataHandler::getUserStatus(int64 uid, bool load)
{
    LoadStatus status = LOAD_INVALID;

    User* user = getUser(uid, &status, load);
    // found
    if (user != NULL)
    {
        return LOAD_SUCCESS;
    }
    // remote user
    if (status == LOAD_MISS)
    {
        return LOAD_MISS;
    }
	// loading data
    if (status == LOAD_WAITING)
    {
        return LOAD_WAITING;
    }
    // not found or error
    return LOAD_ERROR;
}

User* GameDataHandler::getUser(const string& pid, LoadStatus* status, bool load)
{
    LoadStatus tmp_status, *status_ = (status == NULL) ? &tmp_status : status;
    if (!isPlatidLocal(pid))
    {
        *status_ = LOAD_MISS;
        return NULL;
    }

    hash_map<string, int64>::iterator iter = m_mapPlatId2Uid.find(pid);
    if (iter != m_mapPlatId2Uid.end())
    {
        *status_ = LOAD_SUCCESS;
        int64 uid = iter->second;
        if (uid == 0)
        {
            *status_ = LOAD_EMPTY;
            return NULL;
        }
        if (uid == -1)
        {
            *status_ = LOAD_WAITING;
            return NULL;
        }
        return getUser(uid, status_, load);
    }
    int64 uid = -1ll;
    m_mapPlatId2Uid.insert(make_pair(pid, uid));
    loadPlatidMap(pid);
    *status_ = LOAD_WAITING;
    return NULL;
}

User* GameDataHandler::getOnlineUser(int64 uid)
{
    User* user = getUser(uid, NULL, false);
    if (!user || !user->Online())
        return NULL;
    return user;
}

vector<User*> GameDataHandler::getOnlineUsers(const vector<int64>& uids)
{
    vector<User*> users;
    for (size_t i = 0; i < uids.size(); i++)
    {
        User* user = getOnlineUser(uids[i]);
        if (user)
            users.push_back(user);
    }
    return users;
}

vector<User*> GameDataHandler::getAllOnlineUsers()
{
    vector<User*> users;
    for (hash_map<int64, User*>::iterator it = m_mapUsers.begin(); it != m_mapUsers.end(); it++)
    {
        User* user = it->second;
        if (user && user->Online())
            users.push_back(user);
    }
    return users;
}

int64 GameDataHandler::getAllOnlineUserNum()
{
	int64 nCount = 0;
	for (hash_map<int64, User*>::iterator it = m_mapUsers.begin(); it != m_mapUsers.end(); it++)
	{
		User* user = it->second;
		if (user && user->Online())
		{
			nCount++;
		}
	}
	return nCount;
}

User* GameDataHandler::createUser( const string& pid )
{
    LOCALE_TYPE locale = ServerConfig::Instance().GetServerLocale();
    CHANNEL_TYPE channel = ServerConfig::Instance().GetServerChannel();
	int regionId = ServerConfig::Instance().gameregion(nid_);
    int auto_inc = static_cast<int>(next_user_id_++);
    int64 uid = distribution::allocUid(static_cast<int>(locale), static_cast<int>(channel), regionId, auto_inc);
    User* user = new User(uid, pid);
	user->InitNewUser();

    mapUidToUser(uid, user);
    mapPlatidToUid(pid, uid);

    savePlatidMap(pid, uid);
    saveUserData(user);
	SaveGlobalStarInfo();
    return user;
}

bool GameDataHandler::LoadUserFromMemCache( int64 nUserID )
{
	string infostr("");
	bool hasInfo = GameCacheInst::instance().GetUserInfo( nUserID, infostr);
	if (hasInfo)
	{
		User* pUser = new User();
		DB_User dbUser;
		if (dbUser.ParseFromString( infostr ))
		{
			pUser->SetDbUser(dbUser);
			updateLoadUser(nUserID, pUser);
			return true;
		}
		else
		{
			delete pUser;
			LOG4CXX_ERROR(logger_, "GameDataHandler::LoadUserFromMemCache() ERROR!!! ParseFromString UserID:" << nUserID);
			return false;
		}
	}
	return false;
}

void GameDataHandler::LoadPlatFromBack(const string& pid)
{
	m_pDataSaver->GMloadMapFromBack(this, pid);
}

User* GameDataHandler::TestGetUser( int64 uid, LoadStatus* status /*= NULL*/ )
{
	LoadStatus tmp_status, *status_ = (status == NULL) ? &tmp_status : status;
	hash_map<int64, User*>::iterator iter = m_mapUsers.find(uid);
	if (iter != m_mapUsers.end())
	{
		if (iter->second == NULL)
		{
			*status_ = LOAD_EMPTY;
			return NULL;
		}
		*status_ = LOAD_SUCCESS;
		return iter->second;
	}

	map<int64, LoadStatus>::iterator load_iter = m_mapLoadList.find(uid);
	if (load_iter != m_mapLoadList.end())
	{
		*status_ = LOAD_WAITING;
		return NULL;
	}

	// max wait for 10000
	int load_count = m_mapLoadList.size();
	if (load_count >= MAX_LOAD_WAITING)
	{
		*status_ = LOAD_BUSY;
		return NULL;
	}

	m_mapLoadList.insert(make_pair(uid, LOAD_WAITING));
	*status_ = LOAD_WAITING;
	loadUserData(uid);
	return NULL;
}