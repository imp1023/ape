#pragma once

#include <list>
#include <algorithm>
#include <pthread.h>
#include "../common/const_def.h"
#include "../common/ServerConfig.h"
#include "countryDB.pb.h"

#include <log4cxx/logger.h>
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#else
#include <tcrdb.h>
#endif

#define HotCacheErrFile "hc_err"

using namespace std;

class CountryDataHandler;

extern ServerConfig serverConfig;


struct CityDBItem
{
	CountryDataHandler* dh;
	string key;
	string* data;
//	int64 uid;
};

enum ActionType 
{
	ACTION_COUNTRY_LOAD = 2,
	ACTION_COUNTRY_SAVE = 3
};


class CountryDataSaver 
{
public:
	CountryDataSaver(int nid);
	~CountryDataSaver();

	bool	initMutex();
	bool	termMutex();

	void	initThreads();
	void	termThreads();

	void	initDatabase();
	void	termDatabase();

	//return succ
	int		safeSaveAll(time_t revision, CountryDataHandler* const dh, bool force=false);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_City *pDBCity);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_Country *pDBCountry);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_GuildList *pDBGuildLst);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_UserList *pDBUserLst);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_ResourceData *pDBResourceData);
	int		safeSave(CountryDataHandler* const dh, int nKey,DB_C_GvgCity *pDBCity);
    int     safeSave(CountryDataHandler* const dh, int nKey,DB_C_WarGameData *pDBWarGameData);
	int     safeSave(CountryDataHandler* const dh, int nKey,DB_C_BossBtlMsg *pDBBossBtlMsg);

	void	routine(ActionType type);

private:
	int		safeSaveAllCity(time_t revision, CountryDataHandler* const dh, bool force=false);
	int		safeSaveAllCountry(time_t revision, CountryDataHandler* const dh, bool force=false);
	int		safeSaveAllGuildLst(time_t  revision,CountryDataHandler* const dh, bool force=false);
	int		safeSaveAllUserLst(time_t  revision,CountryDataHandler* const dh, bool force=false);
	int		safeSaveAllResourceData(time_t  revision,CountryDataHandler* const dh, bool force=false);
	int		safeSaveAllGvgCity(time_t revision, CountryDataHandler* const dh, bool force=false);
    int     safeSaveAllWarGameData(time_t  revision,CountryDataHandler* const dh, bool force=false);
	int     safeSaveAllBossBtlMsg(time_t  revision,CountryDataHandler* const dh, bool force=false);
public:
	int		loadCity(const string& strKey,DB_C_City& dbCity);
	int		loadCountry(const string& strKey,DB_C_Country& dbCountry);
	int		loadGuildLst(const string& strKey,DB_C_GuildList& dbGuildLst);
	int		loadUserLst(const string& strKey,DB_C_UserList& dbUserLst);
	int		loadResourceData(const string& strKey,DB_C_ResourceData& dbResourseData);
	int		loadGvgCity(const string& strKey,DB_C_GvgCity& dbCity);
    int     loadWarGameData(const string& strKey,DB_C_WarGameData& dbWarGameData);
	int     loadBossBtlMsg(const string& strKey,DB_C_BossBtlMsg& dbBossBtlMsg);
protected:

	int		saveInfo2DB(CountryDataHandler* const dh, string strKey, string& data);
	int		pushSaveCountry(CountryDataHandler* const dh, int nKey,DB_C_Country* pDBCountry);
	int		pushSaveCity(CountryDataHandler* const dh, int nKey,DB_C_City* pDBCity);
	int		pushSaveGuildLst(CountryDataHandler* const dh,int nKey,DB_C_GuildList* pDBGuildLst);
	int		pushSaveUserLst(CountryDataHandler* const dh,int nKey,DB_C_UserList* pDBUserLst);
	int		pushSaveResourceData(CountryDataHandler* const dh,int nKey,DB_C_ResourceData* pDBResourceData);
	int		pushSaveGvgCity(CountryDataHandler* const dh, int nKey,DB_C_GvgCity* pDBCity);
    int     pushSaveWarGameData(CountryDataHandler* const dh, int nKey,DB_C_WarGameData* pDBWarGameData);
	int     pushSaveBossBtlMsg(CountryDataHandler* const dh, int nKey,DB_C_BossBtlMsg* pDBBossBtlMsg);

	static void* saveMapThread(void* arg);
	static void* loadMapThread(void* arg);

	void	writeCountrySaveListLength(int len);
	void	writeSaveError(const char *strError,const char* szKey,int nValueLen);

	TCRDB*	getDb(const string& addr, int port);
	TCRDB*	getCountryLoadDb(int dbid);
	TCRDB*	getCountrySaveDb(int dbid);

	//game DB相关
	TCRDB*	getCountryLoadDb4HotCache(int dbid);
	TCRDB*	getCountrySaveDb4HotCache(int dbid);

	int		saveInfo2HotCache(CountryDataHandler* const dh,string strKey , string& data);
	char*   loadInfoFromHotCache(const string& strKey,int& len);
private:
	int		GetDBIDByKey(string strKey);
protected:
	bool	b_serving;

	pthread_t th_map_load;
	pthread_t th_map_save;

	pthread_mutex_t load_map_mutex_;
	pthread_mutex_t save_map_mutex_;

	list<CityDBItem> save_map_list_;
	list<CityDBItem> load_map_list_;

	int db_num;
	//主数据库
	map<int, void *> db_country_load;
	map<int, void *> db_country_save;

	map<int, void *> hotcache_country_load;
	map<int, void *> hotcache_country_save;


	log4cxx::LoggerPtr logger_;
	int nid_;

	time_t log_plattime;

	int m_nDirtyCityCnt;
	int m_nDirtyCountryCnt;
	int m_nDirtyGuildLstCnt;
	int m_nDirtyUserLstCnt;
	int m_nDirtyResourceDataCnt;
	int m_nDirtyGvgCityCnt;
	int m_nDirtyWarGameDataCnt;
    int m_nDirtyBossBtlMsgCnt;

	bool m_bHotCacheEnable;
};
