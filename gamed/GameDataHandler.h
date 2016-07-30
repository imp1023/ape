#ifndef _GAMEDATAHANDLER_H_
#define _GAMEDATAHANDLER_H_
#pragma once
#include <log4cxx/logger.h>
#include "GameRgnDataHandler.h"

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

class FriendInfoLite;
class CLiteCountry;

class GameFriendInfo
{
public:
	GameFriendInfo();
	~GameFriendInfo();
	void Clear();

    //int64 timestamp;
    //int64 fid;
	FriendInfoLite* m_pFrdInfo;
	string	m_strName[PLAT_TYPE_MAX];	
	string	m_strProfileLink[PLAT_TYPE_MAX];
	bool	m_bLoad[PLAT_TYPE_MAX];		
	map<int,int> m_mapRegionLV;
	map<int,int64> m_mapRegionUID;
} ;

class GameDataSaver;
class GameEventHandler;
class Clock;
class Event;

#define MAX_LOAD_WAITING 10000
const float FROZEN_RATE = 0.5;

class GameDataHandler : public DataHandler
{
public:
    GameDataHandler(int nid);
    virtual ~GameDataHandler(void);
	void				SetEventHandler(GameEventHandler* eh){eh_ = eh;}
	GameEventHandler*	GetEventHandler() {return eh_;};

    virtual void tick();
    virtual void quit();

	virtual void termsThread();

    void markUserDirty(User* user);

	void UpdateRemoveUser_new();
	void UpdateRemoveUser();
	void RemoveUser(User *pUser);


    void saveAllUserData(bool urgent = false);
    void saveUserData(User* user);
    void loadUserData(int64& uid);
    void updateLoadUser(int64& uid, User* user);
    void updateUserMap();

	void updateUser(GameEventHandler * eventHandler);


    //bool isPlatidLocal(const string& pid);
    bool isUidLocal(int64 uid);
	int	 getPhysicsGameNum();
    LoadStatus getUserStatus(int64 uid, bool load = false);

	int getBandUser(){return m_bandUser;}
	void setBandUser(int BandUser){m_bandUser=BandUser;}

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

    void updateUser(User* user, const string &name, const string &profile_link,
                    int gender, PLAT_TYPE plat_type, bool bIsYellowDmd, bool bIsYellowDmdYear,
                    int i4YellowDmdLv, const vector<string> &fpid,int nCity,bool isHighYellowDmd,GameEventHandler* eh_,bool isHighDmdYear,int , int , int , int nHighBlueYearTime);
    User* createUser(int64 uid,const string& pid, const string &name, const string& profile_link,
                     int gender, PLAT_TYPE plat_type, bool bIsYellowDmd, bool bIsYellowDmdYear,
                     int i4YellowDmdLv, const vector<string> &fpid,int nRegion,int nCity,bool bIsNewUser,string strVIA,bool isHighYellowDmd,GameEventHandler* eh_,string strChannel,bool isHighDmdYear, int, int, int, int nHighBlueYearTime,
					 int nID, int nName, int nType, string strSku);
    User* newUser(int64 uid, const string& pid, const string &name,
                  const string &profile_link, int gender, PLAT_TYPE plat_type,
                  bool bIsYellowDmd, bool bIsYellowDmdYear, int i4YellowDmdLv,
                  const vector<string> &fpid,int nRegion,int nCity,bool isHighYellowDmd,GameEventHandler* eh_,bool isHighDmdYear, int, int, int, int nHighBlueYearTime,
				  int nID, int nName, int nType, string strSku);

    User* getUser(int64 uid, LoadStatus* status = NULL, bool load = true);
	int64 GetUserID(const string& pid,int nRegion)
	{
		if(nRegion<0||nRegion>=MAX_REGION_CNT)
			return 0;
		return m_mapUid[nRegion][pid];
	}
	void AddUserID(const string& pid,int nRegion,int64& uid)
	{
		if(nRegion<0||nRegion>=MAX_REGION_CNT)
			return;
		m_mapUid[nRegion][pid] = uid;
	}


    hash_map<int64, User*>& getUsers(void)
    {
        return users_;
    }
	map<int64,int>& getOnlineUsersID()
	{
		return onlineUsersID_;
	}
	//void AddBroadCast2Lst(const RseComBroadcast* pBroadcast);

	void AddLocalStarInfo(User* pUser,int nRegion);		//将玩家信息放入本地卫星列表
	void DelLocalStarInfo(int ntype,int64 nID,int nRegion);
	void GetStartUserlist(list<int64>& lstLv,int nNum,int nlv,Star_User_Group emGroup,int64 StarIDLst[500],int& retNum,int nRegion);
    GameFriendInfo* getFriendInfo(const string& platID, enum PLAT_TYPE i4PlatType);
    //GameFriendInfo* getFriendInfo(const int64 uid, enum PLAT_TYPE i4PlatType);
    void setFriendInfo(const string& platID, int64& uid, FriendInfoLite* pLite, enum PLAT_TYPE i4PlatType,int nRegion,int nLV);

	void PushOnlineUserID(int64 nID);
	void PopOnlineUserID(int64 nID);
	int getGuildSrvIDByGuildID(int uid);
    int getGamedIdByUserId(int64 uid);
	int getGameIdfromPlatId(string const& platid,int nRegion);
	time_t GetSrvRootTime(){return m_ltStart;}

	const hash_map<int64, User*> & getDirtyUsers()
	{
		return dirty_users_;
	}
	
	void FillGuildInfoToEvent(Event* e, User* pUser);

	bool LoadUserFromMemCache( int64 nUserID );

	bool CanLoadByUser(User* pUser,int64 nTID,int nEnterType);

	int GetServerId() {return nid_;}

	void SetAdFlag(bool bFlag){m_bAdFlag = bFlag;}
	bool AdFlag(){return m_bAdFlag;}

public:
	//国家相关
	bool NeedGetCountryUserRank(time_t ltNow);
public:
	//分区相关
	GameRgnDataHandler* GetRgnDataHandler(int nRegion);

public:
	//跨区国战
	CLiteCountry* GetLiteRegionCountryInfo(int nCountry,bool bCreateIfNotExit = false);
	void SetRegionCountryUser(int nCountry,int nCnt);
	int  GetRegionCountryUser(int nCountry);
	void AddRegionCountryUser(int nCountry,int nCnt);		
	int  GetMinUserRegionCountry();	//获得最少人数的国家
	int  GetMaxUserRegionCountry();	//获得最多人数的国家
	int  GetRegionCountryBoom(int nCountry);	//获得国家繁荣度
	void ResetRegionCountryRank();

public:
    //超级强攻令
    void DuleSuperStormRemoveUser();

    int AddSuperStormRemoveUser(int64 lUid, int tRemoveTime);

    void RemoveSuperStormUser(int64 lUid);

private:
    void mapUidToUser(int64& uid, User* user);

    log4cxx::LoggerPtr logger_;

	time_t m_ltStart;

    int64 next_user_id_;
    int64 next_item_id_;


    hash_map<int64, User*> users_;
	map<int64,int> onlineUsersID_;

   // hash_map<string, int64> platid_uid_;
	hash_map<string, int64> m_mapUid[MAX_REGION_CNT];
    map<int64, LoadStatus> load_list_;
    map<int64, User*> loaded_user_;
	//map<int64, DB_NPCList*> loaded_npc_;
	//map<int64, DB_Effect*>  loaded_effect_;
   // map<string, int64> loaded_platid_;
    hash_map<int64, User*> dirty_users_;
	list<int64>			m_listRemove;	//删除用户列表
	hash_map<int64,int> m_mapRemove;//删除用户列表，用于排差
	int64				m_nRemoveKey; //删除玩家标记
	
	//分区相关
	//map<string, DB_PlatidInfoList*> platid_uid_;
	//map<string, DB_PlatidInfoList*> loaded_platid_;
	//map<string, DB_PlatidInfoList*> dirty_platlst_;
	//map<string, int64> dirty_platlst_revision_;

	hash_map<string, GameFriendInfo*> platid_friendinfo_;
 //  hash_map<string, GameFriendInfo*> platid_friendinfo_[PLAT_TYPE_MAX];
 //   hash_map<int64, string> friendinfo_platid_uid_[PLAT_TYPE_MAX];

	GameEventHandler* eh_;
    GameDataSaver* ds_;
    int nid_;
    pthread_mutex_t data_mutex_;

    time_t	timeUpdate;
	time_t	m_timeRemoveUpdate;
	time_t  m_ltStart4RM; //game 启动时间
	//time_t	m_ltGetUserRankTime;	//应获取国家排行榜时间

	bool	m_bAdFlag;	//广告标记
	//deque<string>	m_bulltin;
	//Statistics stat_;
	//Counter counter_;

	// 禁掉外挂玩家
	int    m_bandUser;
private:
	map<int,GameRgnDataHandler*> m_mapRgnDataHandler;
private:
	map<int,CLiteCountry*>		m_mapLiteRegionCountry;

public:
    friend class GameDataSaver;
    
};

#endif
