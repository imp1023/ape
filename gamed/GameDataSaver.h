#ifndef _GAMEDATASAVER_H_
#define _GAMEDATASAVER_H_
#pragma once

#include <list>
#include <algorithm>
#include <pthread.h>
#include <log4cxx/logger.h>
#include "../common/const_def.h"
#include "../common/ServerConfig.h"
#include "../common/db_util.h"

using namespace std;

class GameDataHandler;

struct UserItem
{
	GameDataHandler* dh;
	int64 uid;
	string* data;
};

struct MapItem
{
	GameDataHandler* dh;
	string platid;
	int64 uid;
};

enum ActionType 
{
	ACTION_USER_LOAD = 0,
	ACTION_USER_SAVE = 1,
	ACTION_MAP_LOAD = 2,
	ACTION_MAP_SAVE = 3
};

class User;

class GameDataSaver 
{
public:
	GameDataSaver(int nid);
	~GameDataSaver();

	bool	initMutex();
	bool	termMutex();

	void	initThreads();
	void	termThreads();

	void	initDatabase();
	void	termDatabase();

	void	loadAllocateSetting(int64& uid, int64& iid);
	void	saveAllocateSetting(int64& uid, int64& iid);

	void	LoadGlobalStarInfo();
	void	SaveGlobalStarInfo();

	//return succ
	int		safeSaveAllUser(time_t revision, GameDataHandler* const dh, bool force=false);
	int		safeSaveUser(GameDataHandler* const dh, User* user);
	int		safeLoadUser(GameDataHandler* const dh, int64& uid);
	int		safeSaveAllMap(GameDataHandler* const dh);
	int		safeSaveMap(GameDataHandler* const dh, const string& pid, int64& uid);
	int		safeLoadMap(GameDataHandler* const dh, const string& pid);

	void	routineUser(ActionType type);
	void	routineMap(ActionType type);

protected:
	void	MakeUserString(GameDataHandler *dh, User *pUser, string &oString );
	void	LoadUserFromString(GameDataHandler *dh, User *pUser, string &iString);

	int		saveUser(GameDataHandler* const dh, int64& uid, string& data);
    int     saveUserBackup(GameDataHandler* const dh, int64&uid, string& data);
	int		loadUser(GameDataHandler* const dh, int64& uid);
	int		saveMap(GameDataHandler* const dh, const string& pid, int64& uid);
    int     saveMapBackup(GameDataHandler* const dh, const string&pid, int64& uid);
	int		loadMap(GameDataHandler* const dh, const string& pid);
public:
	void	RemovePlatidFromDb(const string& pid);
	void	RemoveUserFromDb(int64 uid);
	int		GMloadMapFromBack(GameDataHandler* const dh, const string& pid);
protected:
	int		pushSaveUser(GameDataHandler* const dh, int64 uid, User* user);
	int		pushLoadUser(GameDataHandler* const dh, int64& uid);
	int		pushSaveMap(GameDataHandler* const dh, const string& pid, int64& uid);
	int		pushLoadMap(GameDataHandler* const dh, const string& pid);
	
	static void* saveUserThread(void* arg);
	static void* loadUserThread(void* arg);
	static void* saveMapThread(void* arg);
	static void* loadMapThread(void* arg);

	void	writeSaveListLength(int len);
	void	writeSaveError(const char *strError);
    void    writeDisconnectError(const char* strError);

	TCRDB*	getDb(const string& addr, int port);
	TCRDB*	getUserLoadDb(int dbid);
	TCRDB*  getUserLoadOldDb(int dbid);
	TCRDB*  getUserLoadBackDb(int dbid);
	TCRDB*	getUserSaveDb(int dbid);
    TCRDB*  getUserSaveDbBackup(int dbid);
	TCRDB*	getPlatLoadDb(int dbid);
	TCRDB*  getPlatLoadBackDb(int dbid);
	TCRDB*	getPlatSaveDb(int dbid);
    TCRDB*  getPlatSaveDbBackup(int dbid);
	TCRDB*	getAllocateDb();

	TCRDB*  getStarDb();
public:
	int	    getDBId(int64 uid);
    int     getDBId(const string& platid);
protected:
	bool			b_serving;

	pthread_t		m_threadLoadUser;
	pthread_t		m_threadSaveUser;
	pthread_t		m_threadLoadMap;
	pthread_t		m_threadSaveMap;

	pthread_mutex_t m_mutexLoadUser;
	pthread_mutex_t m_mutexSaveUser;
	pthread_mutex_t m_mutexLoadMap;
	pthread_mutex_t m_mutexSaveMap;

	list<UserItem>	m_listSaveUser;
	list<UserItem>	m_listLoadUser;
	list<MapItem>	m_listSaveMap;
	list<MapItem>	m_listLoadMap;

	int db_num;
	map<int, void *> db_user_load;
	map<int, void *> db_user_save;
    map<int, void *> db_user_save_backup;
	map<int, void *> db_user_load_old;
	map<int, void *> db_plat_load;
	map<int, void *> db_plat_save;
    map<int, void *> db_plat_save_backup;
	map<int, void *> db_plat_load_back;
	map<int, void *> db_user_load_back;

	log4cxx::LoggerPtr logger_;
	int nid_;
	char allocate_file[256];
	char allocate_dbstr[256];
	char GlobalStarInfo_DB[256];
	TCRDB* alloc_rdb;
	TCRDB* star_rdb;
	time_t log_time;
    time_t log_error_time;
	int count_dirty_;
};

#endif
