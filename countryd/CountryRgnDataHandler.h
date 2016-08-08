#pragma once

#include <log4cxx/logger.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>

#include <pthread.h>
#include "../event/DataHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../common/Rand.h"
#include "countryDB.pb.h"
#include "../event/RseQueryStarInfo.pb.h"
#include "../event/RseUpdateAlliances.pb.h"

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

typedef struct{
	string id;
	string pictureUrl;
	string name;
	int score;
	string role;
	int currentWarScore;
}Members;

class CUser
{
public:
	CUser(DB_C_UserLite *pUser){m_pDbUser = pUser;}
	~CUser(){}

	void SetUID(int64 uid){m_pDbUser->set_uid(uid);}
	int64 GetUID(){return m_pDbUser->uid();}
	void SetName(string strName){m_pDbUser->set_name(strName);}
	string GetName(){return m_pDbUser->name();}
	void SetURL(string strURL){m_pDbUser->set_url(strURL);}
	string GetURL(){return m_pDbUser->url();}
	int GetScore(){return m_pDbUser->score();}
	int GetDmgProtectionTimeLeft(){return m_pDbUser->damageprotectiontimeleft();}
	int GetTutorialCompleted(){return m_pDbUser->tutorialcompleted();}
	int GetOnline(){return m_pDbUser->isonline();}
	int GetLevelOnScore(){return m_pDbUser->levelbasedonscore();}
	int GetXP(){return m_pDbUser->xp();}

private:
	DB_C_UserLite *m_pDbUser;
};

class CCity
{
public:
	CCity(int nRegion, DB_C_City *pCity):m_nRegion(nRegion), m_dbCity(pCity)
	{	
		
	}
	~CCity()
	{
		if(m_dbCity){
			m_dbCity->Clear();
			m_dbCity = NULL;
		}
	}

	int GetCityID(){return m_dbCity->cityid();}
	void SetCityID(int nCityId){m_dbCity->set_cityid(nCityId);}
	int GetX(){return m_dbCity->x();}
	void SetX(int x){m_dbCity->set_x(x);}
	int GetY(){return m_dbCity->y();}
	void SetY(int y){m_dbCity->set_y(y);}
	int GetName(){return m_dbCity->name();}
	void SetName(int nName){m_dbCity->set_name(nName);}
	int GetRegion(){return m_nRegion;}
	DB_C_City* GetDBCity(){return m_dbCity;}
	void SetType(int nType){m_dbCity->set_type(nType);}
	int GetType(){return m_dbCity->type();}

	int GetPlanetSize(){return m_dbCity->planets_size();}

public:
	DB_C_City *m_dbCity;
	int	m_nRegion;
};

class CCountry
{
public:
	CCountry(int nRegionID)
		:m_nRegion(nRegionID)
	{
		m_mCitys.clear();
		m_dbCountry.set_region(nRegionID);
	}

	~CCountry()
	{
		Clear();
	}

	void Clear()
	{
		m_mCitys.clear();
	}

	int GetKey(){return m_nRegion;}
	DB_C_Country* GetDBCountry(){return &m_dbCountry;}

public:
	DB_C_Country m_dbCountry;
	int m_nRegion;
	map<int, CCity*> m_mCitys;
};

class CountryDataHandler;
class CountryRgnDatHandler 
{//国家分区数据信息
public:
	CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion);
	~CountryRgnDatHandler();

public:
	void InitCountry();
	bool LoadAllCountryInfo();

	void Clear();
	int GetRegion(){return m_nRegion;}

	//void markCityDirty(CCity* pCity);
	void markCountryDirty(CCountry* pCountry);

	CCountry* GetCountry(){return m_pCountry;}
	int GetMaxCityId(){return m_MaxCityID;}

	map<int, CCity*>& GetAllCity(){return m_mapCitys;}
	
	//star
	void GetNewPlayerStar(int64 uid, int nPlanetId, string platId, string url, 
		string &strRspSku, int &nRspName, int &nRspType, int &nStarId);
	void FillinStarInfo(int nStarId, RseQueryStarInfo *rsp);

public:
	CCity*		GetCity(int nCityID);
	int			GetCityKey(int nCityID){return (m_nRegion*MAX_CITY_ID + nCityID);}
	CUser*		GetUser(int uid);

public://联盟相关  hansoul
	DB_C_Alliance* GetAlliance(int aid);
	DB_C_Alliance* GetAllianceByPlayer(int64 playerId);
	int CreateAlliance(string name, string logo, string description, int publicRecruit, string guid, Members* mb);
	bool editAlliance(int allianceId, string logo, string description,RseUpdateAlliances* rseUpdateAlliances);
	bool updatePublic(int allianceId, int publicRecruit,RseUpdateAlliances* rseUpdateAlliances);
	bool getMyAlliance(int allianceId, int attackScore, int warScore, int role, int64 playerId, RseUpdateAlliances* rseUpdateAlliances);
	bool joinAlliance(int allianceId, int64 guid, int level,string name, string pictureUrl, int score,RseUpdateAlliances* rseUpdateAlliances);
	bool refuseJoinAlliance(int allianceId, int64 playerId, int64 guid, RseUpdateAlliances* rseUpdateAlliances);
	bool agreeJoinAlliance(int allianceId, int64 playerId ,int64 guid ,RseUpdateAlliances* rseUpdateAlliances);
	bool leaveAlliance(int allianceId,int64 guid ,RseUpdateAlliances* rseUpdateAlliances);
	bool kickMember(int64 adminId, int64 memberId, RseUpdateAlliances* rseUpdateAlliances);
	bool warHistory(int allianceId, int count, int startIndex, RseUpdateAlliances*  rseUpdateAlliances);
	bool declareWar(int64 guid, int enemyAllianceId, RseUpdateAlliances*  rseUpdateAlliances);
	bool sendMessage(RseUpdateAlliances*  rseUpdateAlliances);
	bool getNews(int64 guid, int count, int fromIndex, RseUpdateAlliances*  rseUpdateAlliances);
	bool getReward(int64 guid, int alliancesBattlesWon,int warPoints, RseUpdateAlliances*  rseUpdateAlliances);
	bool getSuggestedAlliances(int count,RseUpdateAlliances* rseUpdateAlliances);
	bool getAlliances(int64 guid, int count, int from, bool userPage, string searchKey, RseUpdateAlliances* rseUpdateAlliances);
	bool getAlliance(int allianceId, int64 guid, bool includeMembers, RseUpdateAlliances* rseUpdateAlliances);
	bool grantMember(int64 memberId, int role, RseUpdateAlliances* rseUpdateAlliances);
	bool getAlliancesNotInWar(int64 guid, int count, int from, bool userPage, string searchKey, RseUpdateAlliances* rseUpdateAlliances);
	bool CheckAlnName(string name);
	void FillinAln(int alnId, RseUpdateAlliances* rse, bool includeMembers = true);
	DB_C_AliMember*	GetAllianceMember(int allianceId, int64 playerId);
	void DisMissAlliance(int allianceId);
	void RemoveMember(int allianceId, int64 playerId);
	bool CheckAlnInWar(int allianceId);
	void FillNews(DB_C_AllianceNews* pNews, RseUpdateAlliances*  rseUpdateAlliances);
private:
	CountryDataHandler* m_pDH;
	CCountry*			m_pCountry;
	int					m_nRegion;
	map<int, CCity*>	m_mapCitys;
	map<int, CUser*>    m_mapUsers;
	map<int, DB_C_Alliance*> m_mapAlliances;
	vector<DB_C_Alliance*> m_allianceByRank;
	int m_MaxCityID;
};