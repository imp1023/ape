#ifndef _PLATDATASAVER_H_
#define _PLATDATASAVER_H_
#pragma once

#include <list>
#include <algorithm>
#include <pthread.h>
#include "../common/const_def.h"
#include "../common/ServerConfig.h"
#include "platDB.pb.h"
#include <log4cxx/logger.h>
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#else
#include <tcrdb.h>
#endif

#define HotCacheErrFile "hc_err"

using namespace std;

class PlatDataHandler;

extern ServerConfig serverConfig;


struct MapItem
{
	PlatDataHandler* dh;
	string platid;
	string* data;
//	int64 uid;
};

enum ActionType 
{
	ACTION_MAP_LOAD = 2,
	ACTION_MAP_SAVE = 3
};


class PlatDataSaver 
{
public:
	PlatDataSaver(int nid);
	~PlatDataSaver();

	bool	initMutex();
	bool	termMutex();

	void	initThreads();
	void	termThreads();

	void	initDatabase();
	void	termDatabase();

	void	loadAllocateSetting(int64& uid, int64& iid);
	void	saveAllocateSetting(int64& uid, int64& iid);

	//return succ
	int		safeSaveAllMap(time_t revision, PlatDataHandler* const dh, bool force=false);
	int		safeSaveMap(PlatDataHandler* const dh, DB_Plat *pDBPlat);
	int		safeLoadMap(PlatDataHandler* const dh, const string& pid);

	void	routineMap(ActionType type);

protected:
	// platid信息序列化为字符串
	void	MakePlatidString(PlatDataHandler *dh, DB_Plat *pDBPlat, string &oString );
	void	LoadPlatidFromString(PlatDataHandler *dh, DB_Plat *pDBPlat, string &iString);

	int		saveMap(PlatDataHandler* const dh, const string& pid, string& data,int type);
	int		loadMap(PlatDataHandler* const dh, const string& platid,int type);

	int		pushSaveMap(PlatDataHandler* const dh, DB_Plat* pPlatidinfoList);
	int		pushLoadMap(PlatDataHandler* const dh, const string& pid);
	
	static void* saveMapThread(void* arg);
	static void* loadMapThread(void* arg);

	void	writePlatSaveListLength(int len);
	void	writeSaveError(const char *strError,const char* szKey,int nValueLen);

	TCRDB*	getDb(const string& addr, int port);
	TCRDB*	getPlatLoadDb(int dbid,int type);
	TCRDB*	getPlatSaveDb(int dbid,int type);
	TCRDB*	getAllocateDb(int type);

	//game DB相关
	TCRDB*	getPlatLoadDb4HotCache(int dbid);
	TCRDB*	getPlatSaveDb4HotCache(int dbid);

	int		saveMap2HotCache(PlatDataHandler* const dh, const string& pid, string& data);
	char*   loadMapFromHotCache(PlatDataHandler* const dh, const string& platid,int& len);
private:
protected:
	bool	b_serving;

	pthread_t th_map_load;
	pthread_t th_map_save;

	pthread_mutex_t load_map_mutex_;
	pthread_mutex_t save_map_mutex_;

	list<MapItem> save_map_list_;
	list<MapItem> load_map_list_;

	int db_num;
	//主数据库
	map<int, void *> db_plat_load;
	map<int, void *> db_plat_save;
	//备数据库
	map<int, void *> db_plat_load2;
	map<int, void *> db_plat_save2;


	map<int, void *> hotcache_plat_load;
	map<int, void *> hotcache_plat_save;


	log4cxx::LoggerPtr logger_;
	int nid_;
	char allocate_file[256];
	char allocate_dbstr[256];
	
	TCRDB* alloc_rdb;
	TCRDB* alloc_rdb2;

	time_t log_plattime;

	int count_dirty_plat_;

	bool m_bHotCacheEnable;
	bool m_bHotCacheEnableLoad;
	bool m_bHotCacheEnableLoadNPC;
};

#endif
