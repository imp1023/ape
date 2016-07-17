#ifndef _GAMEDATAHANDLER_H_
#define _GAMEDATAHANDLER_H_
#pragma once
#include <log4cxx/logger.h>
#include "../common/const_def.h"
#include <vector>
#include <map>
#include <list>
#include <pthread.h>
#include "../event/DataHandler.h"
using namespace std;
#ifndef WIN32
#include <ext/hash_map>
using namespace __gnu_cxx;
namespace __gnu_cxx
{

	template<>
	struct hash<long long>
	{

		size_t
			operator()(long long __x) const
		{
			return __x;
		}
	} ;

	template<>
	struct hash<std::string>
	{

		size_t
			operator()(std::string __s) const
		{
			return __stl_hash_string(__s.c_str());
		}
	} ;
}
#else
#include <hash_map>
using namespace stdext;
#endif

enum LoadStatus
{
    LOAD_INVALID = 0,
    LOAD_MISS = 1,
    LOAD_WAITING = 2,
    LOAD_HANDLING = 3,
    LOAD_SUCCESS = 4,
    LOAD_EMPTY = 5,
    LOAD_BUSY = 6,
    LOAD_ERROR = 99
};

class User;
class GameDataSaver;
class Clock;
class Event;

#define MAX_LOAD_WAITING 10000
const float FROZEN_RATE = 0.5;

class GameDataHandler : public DataHandler
{
public:
    GameDataHandler(int nid);
    virtual ~GameDataHandler(void);

    virtual void init();
    virtual void tick();
    virtual void quit();

	virtual void termThreads();

    void	loadAllocateSetting();
    void	saveAllocateSetting();

	void	LoadGlobalStarInfo();
	void	SaveGlobalStarInfo();

    int64	allocateItemID(int num = 1);

    void	markUserDirty(User* user);

    void	saveAllUserData(bool urgent = false);
    void	saveUserData(User* user);
    void	loadUserData(int64& uid);
    void	updateLoadUser(int64& uid, User* user);
    void	updateUserMap();

	void	UpdateRemoveUser();
	void	RemoveUser(User *pUser);
	void	RemovePlatidMap(const string& pid);
	void	RemovePlatidFromDb(const string& pid);
	void	RemoveUserFromDb(int64 uid);

    void	savePlatidMap(const string& pid, int64& uid);
    void	loadPlatidMap(const string& pid);
    void	updateLoadPlatid(const string& pid, int64& uid);
    void	updatePlatidMap();

    bool	isPlatidLocal(const string& pid);
    bool	isUidLocal(int64 uid);
    LoadStatus getUserStatus(int64 uid, bool load = false);

    inline pthread_mutex_t* getDataMutex();
    inline void acquireDataLock();
    inline void releaseDataLock();

	User* createUser(const string& pid);

    User*	getUser(int64 uid, LoadStatus* status = NULL, bool load = true);
    User*	getUser(const string& pid, LoadStatus* status = NULL, bool load = true);
	User*	TestGetUser(int64 uid, LoadStatus* status = NULL);

    User*   getOnlineUser(int64 uid);
    vector<User*> getOnlineUsers(const vector<int64>& uids);
    vector<User*> getAllOnlineUsers();

	int64	getAllOnlineUserNum();

    const hash_map<int64, User*>&	getUsers(void) const;
	const map<int64, User*>&		getDirtyUsers(void) const;

	bool LoadUserFromMemCache( int64 nUserID );
	void LoadPlatFromBack(const string& pid);
private:
    void mapUidToUser(int64& uid, User* user);
    void mapPlatidToUid(const string& pid, int64& uid);

    log4cxx::LoggerPtr logger_;

    int64	next_user_id_;
    int64	next_item_id_;

    hash_map<int64, User*>	m_mapUsers;

    hash_map<string, int64> m_mapPlatId2Uid;
    map<int64, LoadStatus>	m_mapLoadList;
    map<int64, User*>		m_mapLoadedUser;
    map<string, int64>		m_mapLoadedPlatId;
    map<int64, User*>		m_mapDirtyUsers;

	list<int64>				m_listRemove;

    GameDataSaver* m_pDataSaver;
    int nid_;
    pthread_mutex_t m_mutexData;

    time_t	timeUpdate;
	time_t	m_timeRemoveUpdate;
public:
    friend class GameDataSaver;
    friend class GMHandler;
};

////////////////////inline function//////////////////////
inline pthread_mutex_t* GameDataHandler::getDataMutex()
{
	return &m_mutexData;
}

inline void GameDataHandler::acquireDataLock()
{
	pthread_mutex_lock(&m_mutexData);
}

inline void GameDataHandler::releaseDataLock()
{
	pthread_mutex_unlock(&m_mutexData);
}

inline const hash_map<int64, User*>& GameDataHandler::getUsers(void) const
{
	return m_mapUsers;
}

inline
const map<int64, User*>& GameDataHandler::getDirtyUsers(void) const
{
	return m_mapDirtyUsers;
}
#endif
