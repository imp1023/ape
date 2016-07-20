#pragma once

#include <log4cxx/logger.h>
#include <vector>
#include <map>
#include <list>
#include <pthread.h>
#include "../event/DataHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"

#include "platDB.pb.h"
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
} ;

enum LoadRegionStatus
{
	LOAD_REGION_START = LOAD_BUSY,
	LOAD_REGION_PLATERR,
	LOAD_REGION_REGIONERR,
};
class PlatDataSaver;
class PlatEventHandler;
class Clock;
class Event;

#define MAX_LOAD_WAITING 10000
const float FROZEN_RATE = 0.5;

class PlatDataHandler : public DataHandler
{
public:
    PlatDataHandler(int nid);
    virtual ~PlatDataHandler(void);

    virtual void tick();
    virtual void quit();

	virtual void termsThread();

    void loadAllocateSetting();
    void saveAllocateSetting();

    int64 allocateUserID(const string& platid,int nRegion);
    int64 allocateItemID(int num = 1);

	void markPlatDirty(DB_Plat* pDBPlat);

	void saveAllPlatData(bool urgent = false);
	void savePlatidMap(DB_Plat* pDBPlat);
    void loadPlatidMap(const string& pid);
	void updatePlatidMap();
	bool LoadPlatFromMemCache(const string& pid);
	void UpdateRemovePlat();
	void UpdateRemovePlat_New();

	void CheckDBPlat(DB_Plat* pDBPlat);
	// 更新加载的OpenID信息
	void updateLoadPlatid(const string& pid,DB_Plat* pDBPlat);
	// 获得正确的UId
	int64   GetUid(DB_Plat* pDBPlat,int nRegion);


    bool isPlatidLocal(const string& pid);

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


	// 获得pid到UID的映射关系
	DB_Plat* GetDBPlat(const string& pid,LoadStatus* status = NULL,bool load= true);


    int getGamedIdByUserId(int64 uid);
	int getGameIdfromPlatId(string const& platid,int nRegion);
	int getPlatSrvID(const string& pid);

	GameWorkingStatus setWorkingStatus(int gid, GameWorkingStatus status);
	void		checkAllWorkingStatus();

	void mapPlatidToUid(const string& pid,DB_Plat* pDBPlat);

	int GetUnDealGiftCnt(DB_Plat* pDBPlat);
	int GetUnDealFrdGiftCnt(DB_Plat* pDBPlat);

	bool HasInviteFrd(const string& inviteFrdPlat,const string& invitedFrdPlat);
	void AddInviteFrd(const string& inviteFrdPlat,const string& invitedFrdPlat);

	void PushRemovePlat(string strPlatID,time_t ltRevision);
	DB_PlatRegion* GetDBPlatRegion(string strPlatid , int nRegionid , int& state);
public:
	void SaveFrd2TT(DB_Plat* pDBPlat);	
private:
	
	DB_Plat* mapPlatidToUid(const string& pid, int64 uid,int nregion);

    log4cxx::LoggerPtr logger_;


    int64 next_user_id_;
    int64 next_item_id_;


    map<string, LoadStatus> load_list_;

	map<string,int> m_mapInviteFrd;
	//分区相关
	map<string, DB_Plat*> platid_uid_;
	map<string, DB_Plat*> loaded_platid_;
	map<string, DB_Plat*> dirty_platlst_;
	//hash_map<string, int64> dirty_platlst_revision_;

	//hash_map<string,int64> m_mapPlatRMRevision;
	//list<string> m_listRemove;	//删除plat列表
	//hash_map<string,int> m_mapRemove;//删除plat列表，用于排差
	string m_strRemoveKey;	//删除开始位

    PlatDataSaver* ds_;
    int nid_;
    pthread_mutex_t data_mutex_;

    time_t	timeUpdate;
	time_t	m_timeRemoveUpdate;
	time_t  m_ltStart; //plat启动时间
public:
    friend class PlatDataSaver;

public:
	vector<enum GameWorkingStatus> game_status_;
	bool game_all_ready_;
  
};
