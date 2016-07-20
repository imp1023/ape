#ifndef _GAMEDATASAVER_H_
#define _GAMEDATASAVER_H_
#pragma once

#include <list>
#include <algorithm>
#include <pthread.h>
#include "../common/const_def.h"
#include "../logic/User.h"
#include "../common/ServerConfig.h"

#include <log4cxx/logger.h>
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#else
#include <tcrdb.h>
#endif

#define HotCacheErrFile "hc_err"

using namespace std;

class GameDataHandler;

extern ServerConfig serverConfig;

struct UserItem
{
	GameDataHandler* dh;
	int64 uid;
	string* data;
//	string* effdata;
	string* npcdata;
};

struct MapItem
{
	GameDataHandler* dh;
	string platid;
	string* data;
//	int64 uid;
};

enum ActionType 
{
	ACTION_USER_LOAD = 0,
	ACTION_USER_SAVE = 1,
	ACTION_MAP_LOAD = 2,
	ACTION_MAP_SAVE = 3
};


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

	//void	loadAllocateSetting(int64& uid, int64& iid);
	//void	saveAllocateSetting(int64& uid, int64& iid);

	//return succ
	int		safeSaveAllUser(time_t revision, GameDataHandler* const dh, bool force=false);
	int		safeSaveUser(GameDataHandler* const dh, User* user);
	int		safeLoadUser(GameDataHandler* const dh, int64& uid);
	//int		safeSaveAllMap(time_t revision, GameDataHandler* const dh, bool force=false);
	//int		safeSaveMap(GameDataHandler* const dh, const string& pid, DB_PlatidInfoList *pPlatidList);
	//int		safeLoadMap(GameDataHandler* const dh, const string& pid);

	void	routineUser(ActionType type);
	//void	routineMap(ActionType type);

protected:
	void	MakeUserString(GameDataHandler *dh, User *pUser, string &oString);// ,string &oEffString,string &oNpcString);
	void	LoadUserFromString(GameDataHandler *dh, User *pUser, string &iString/*,DB_NPCList* pNpcList,DB_Effect* pEffect*/);

	// platid信息序列化为字符串
	//void	MakePlatidString(GameDataHandler *dh, DB_PlatidInfoList *pPlatidList, string &oString );
	//void	LoadPlatidFromString(GameDataHandler *dh, DB_PlatidInfoList *pPlatidList, string &iString);

	//void    LoadUserNpcFromString(DB_NPCList* pNpcList, string &iString);
	//void    LoadUserEffectFromString(DB_Effect* pEffect,string &iString);

	int		saveUser(GameDataHandler* const dh, int64& uid, string& data,int type);
	//int     saveUserNPC(GameDataHandler* const dh, int64& uid, string& data);
	//int     saveUserEffect(GameDataHandler* const dh, int64& uid, string& data,int type);
	int		loadUser(GameDataHandler* const dh, int64& uid,int type);
	//int     loadUserNPC(GameDataHandler* const dh, DB_NPCList* pNpcList, int64& uid);
	//int     loadUserEffect(GameDataHandler* const dh,DB_Effect* pEffect, int64& uid,int type);
	//int		saveMap(GameDataHandler* const dh, const string& pid, string& data,int type);
	//int		loadMap(GameDataHandler* const dh, const string& platid,int type);

	int		pushSaveUser(GameDataHandler* const dh, int64 uid, User* user);
	int		pushLoadUser(GameDataHandler* const dh, int64& uid);
	//int		pushSaveMap(GameDataHandler* const dh, const string& pid, DB_PlatidInfoList* pPlatidinfoList);
	//int		pushLoadMap(GameDataHandler* const dh, const string& pid);
	
	static void* saveUserThread(void* arg);
	static void* loadUserThread(void* arg);
	//static void* saveMapThread(void* arg);
	//static void* loadMapThread(void* arg);

	void	writeSaveListLength(int len);
	void	writeSaveError(const char *strError,const char* szKey,int nValueLen);

	TCRDB*	getDb(const string& addr, int port);
	TCRDB*	getUserLoadDb(int dbid,int type);
	TCRDB*	getUserSaveDb(int dbid,int type);
	//TCRDB*	getPlatLoadDb(int dbid,int type);
	//TCRDB*	getPlatSaveDb(int dbid,int type);

	//game DB相关
	TCRDB*	getUserLoadDb4HotCache(int dbid);
	TCRDB*	getUserSaveDb4HotCache(int dbid);
	//TCRDB*	getPlatLoadDb4HotCache(int dbid);
	//TCRDB*	getPlatSaveDb4HotCache(int dbid);

	int		saveUser2HotCache(GameDataHandler* const dh, int64& uid, string& data);
	int     saveUserNPC2HotCache(GameDataHandler* const dh, int64& uid, string& data);

	char*	loadUserFromHotCache(GameDataHandler* const dh, int64& uid,int& len);
	char*   loadUserNPCFromHotCache(GameDataHandler* const dh, int64& uid,int& len);

	//int		saveMap2HotCache(GameDataHandler* const dh, const string& pid, string& data);
	//char*   loadMapFromHotCache(GameDataHandler* const dh, const string& platid,int& len);

	void	DelUserNPCInfo(TCRDB* pDB,int64& uid);
	void	DelUserEffectInfo(TCRDB* pDB,int64& uid);
private:
	//临时代码
	//int64	_TmploadMap(GameDataHandler* const dh, const string& platid);
	//bool	_TmpGetUser(TCRDB* pDB,int64 uid,DB_User& dbUser);
	//int		_TmploadUser(GameDataHandler* const dh, int64& uid);
	//TCRDB* plat_tmp_rdb;
	//TCRDB* user_tmp_rdb;
protected:
	bool	b_serving;

	pthread_t th_user_load;
	pthread_t th_user_save;
	//pthread_t th_map_load;
	//pthread_t th_map_save;

	pthread_mutex_t load_user_mutex_;
	pthread_mutex_t save_user_mutex_;
	pthread_mutex_t load_map_mutex_;
	pthread_mutex_t save_map_mutex_;

	list<UserItem> save_user_list_;
	list<UserItem> load_user_list_;
	//list<MapItem> save_map_list_;
	//list<MapItem> load_map_list_;

	int db_num;
	//主数据库
	map<int, void *> db_user_load;
	map<int, void *> db_user_save;
	//map<int, void *> db_plat_load;
	//map<int, void *> db_plat_save;
	//备数据库
	map<int, void *> db_user_load2;
	map<int, void *> db_user_save2;
	//map<int, void *> db_plat_load2;
	//map<int, void *> db_plat_save2;


	map<int, void *> hotcache_user_load;
	map<int, void *> hotcache_user_save;
	//map<int, void *> hotcache_plat_load;
	//map<int, void *> hotcache_plat_save;


	log4cxx::LoggerPtr logger_;
	int nid_;
	char allocate_file[256];
	char allocate_dbstr[256];
	TCRDB* alloc_rdb;
	TCRDB* alloc_rdb2;

	time_t log_time;
	time_t log_plattime;

	int count_dirty_;
//	int count_dirty_plat_;

	bool m_bHotCacheEnable;
	bool m_bHotCacheEnableLoad;
	bool m_bHotCacheEnableLoadNPC;
};

#endif
