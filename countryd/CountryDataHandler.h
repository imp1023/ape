#pragma once

#include <log4cxx/logger.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>

#include <pthread.h>
#include "../event/DataHandler.h"
#include "./CountryRgnDataHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"

#include "countryDB.pb.h"
using namespace std;

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
} ;


class CountryDataSaver;
class CountryDataHandler : public DataHandler
{
	 friend class CountryDataSaver;
public:
    CountryDataHandler(int nid);
    virtual ~CountryDataHandler(void);

	void InitCountry();
	void Clear();

    virtual void tick();
    virtual void quit();
	
	virtual void termsThread();

	GameWorkingStatus setWorkingStatus(int gid, GameWorkingStatus status);
	void		checkAllWorkingStatus();
	

	CountryRgnDatHandler* GetRegionDatHandler(int nRegionID);
	bool		IsRightRegion(int nRegion) {return find(m_lstRegion.begin(), m_lstRegion.end(), nRegion) != m_lstRegion.end();}
	const vector<int>& GetRegionLst(){return m_lstRegion;}

	int			GetTotalArenaPlayerNum();
	void		CheckClearArenaData();

	int getGamedIdByUserId(int64 uid);
public:
	//数据库相关
	void saveCity(CCity* pCity);	//立刻存储
	void markCityDirty(CCity* pCity);

	void saveCountry(CCountry* pCountry);
	void markCountryDirty(CCountry* pCountry);

	void markUserLstDirty(DB_C_UserList* pDBUserLst);

	void saveGuildLst(DB_C_GuildList* pDBGuildLst);
	void markGuildLstDirty(DB_C_GuildList* pDBGuildLst);

	void saveResourceData(DB_C_ResourceData* pDBResourceData);
	void markResourceDataDirty(DB_C_ResourceData* pDBResourceData);

	void saveGvgCity(CGvgCity* pCity);	//立刻存储
	void markGvgCityDirty(CGvgCity* pCity);

    void saveWarGameSiteData(DB_C_WarGameData * pDBWarGameSite);
    void markWarGameDataDirty(DB_C_WarGameData *pDBWarGameData);

	void saveBossBtlMsgData(DB_C_BossBtlMsg * pDBBossBtlMsg);
	void markBossBtlMsgDataDirty(DB_C_BossBtlMsg *pDBBossBtlMsg);

	bool LoadCityFromDB(string szKey,DB_C_City& dbCity,bool& bError);
	bool LoadCountryFromDB(string szKey,DB_C_Country& dbCountry,bool& bError);
	bool LoadGuildLstFromDB(string szKey,DB_C_GuildList& dbLst,bool& bError);
	bool LoadUserLstFromDB(string szKey,DB_C_UserList& dbLst,bool& bError);
	bool LoadResourceFromDB(string szKey,DB_C_ResourceData& dbResourceData,bool& bError);
	bool LoadGvgCityFromDB(string szKey,DB_C_GvgCity& dbCity,bool& bError);
    bool LoadWarGameFromDB(string szKey,DB_C_WarGameData& dbWarGameData,bool& bError);
	bool LoadBossBtlMsgFromDB(string szKey,DB_C_BossBtlMsg& dbWarBossBtlMsg,bool& bError);
	int	 Getnid(){return nid_;}

private:
	bool LoadDBDataFromMemCache(const string& key,DB_C_City& dbCity);
	bool LoadDBDataFromMemCache(const string& key,DB_C_Country& dbCountry);
	bool LoadDBDataFromMemCache(const string& key,DB_C_GuildList& dbGuildLst);
	bool LoadDBDataFromMemCache(const string& key,DB_C_UserList& dbUserLst);
	bool LoadDBDataFromMemCache(const string& key,DB_C_ResourceData& dbResourceData);
	bool LoadDBDataFromMemCache(const string& key,DB_C_GvgCity& dbCity);
    bool LoadDBDataFromMemCache(const string& Key,DB_C_WarGameData& dbWarGameData);
	bool LoadDBDataFromMemCache(const string& key,DB_C_BossBtlMsg& dbWarBossBtlMsg);

	void saveAllDBData(bool urgent = false);

	inline pthread_mutex_t* getDataMutex()
	{
		return &data_mutex_;
	}
	inline void acquireDataLock()
	{
		pthread_mutex_lock(&data_mutex_);
	}
	inline void releaseDataLock()
	{
		pthread_mutex_unlock(&data_mutex_);
	}
private:
	
    log4cxx::LoggerPtr logger_;

	vector<int> m_lstRegion; //这台服务器处理分区列表
	map<int,CountryRgnDatHandler*> m_mapRegionDH;

	//需要存储的城市列表
	map<int, CCity*>	m_mapDirtyCity;	
	//需要存储的国家列表
	map<int,CCountry*>	m_mapDirtyCountry;
	//需要存储的玩家列表
	map<int,DB_C_UserList*> m_mapDrtyUserLst;
	//需要存储的工会列表
	map<int,DB_C_GuildList*> m_mapDrtyGuildLst;
	//需要存储的资源
	map<int,DB_C_ResourceData*> m_mapDrtyResourceLst;
    //需要存储的军事演习信息
    map<int,DB_C_WarGameData*> m_mapDirtyWarGameLst;
	//需要存储的boss战信息
	map<int,DB_C_BossBtlMsg*> m_mapDirtyBossBtlLst;

	map<int, CGvgCity*>	m_mapDirtyGvgCity;	

    CountryDataSaver* ds_;
    int nid_;
    pthread_mutex_t data_mutex_;

    time_t	timeUpdate;
	time_t  m_ltStart; //plat启动时间

public:
	vector<enum GameWorkingStatus> game_status_;
	bool game_all_ready_;
  
};
