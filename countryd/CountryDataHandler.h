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

	int getGamedIdByUserId(int64 uid);

public:
	//数据库相关
	void saveCountry(CCountry* pCountry);
	void markCountryDirty(CCountry* pCountry);
	bool LoadCountryFromDB(string szKey,DB_C_Country& dbCountry,bool& bError);
	int	 Getnid(){return nid_;}

private:
	bool LoadDBDataFromMemCache(const string& key,DB_C_Country& dbCountry);
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

	vector<int> m_lstRegion;
	map<int,CountryRgnDatHandler*> m_mapRegionDH;

	map<int,CCountry*>	m_mapDirtyCountry;

    CountryDataSaver* ds_;
    int nid_;
    pthread_mutex_t data_mutex_;

    time_t	timeUpdate;
	time_t  m_ltStart;

public:
	vector<enum GameWorkingStatus> game_status_;
	bool game_all_ready_;
};
