#pragma once
#include <log4cxx/logger.h>
#include <vector>
#include <map>
#include <list>
#include "../logic/User.h"
#include "../common/statistics.h"
#include "../common/counter.h"
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

class CLiteCountry
{
public:
	CLiteCountry(int nRegion,int nCountryID)
		:m_nRegion(nRegion)
		,m_nCountryID(nCountryID)
	{
		m_nAtkTargetCity= 0;
		m_nDefTargetCity= 0;
	
		m_nUserCnt		= 0;
		m_nLastDayRank	= 0;	
		m_nCurRank		= 0;
		m_ltUserRankGetTime	= 0;
		m_nBoom			= 0;
		m_nLastDayBoom	= 0;
		m_nGuildCnt		= 0;
		m_nPresidentUid = 0;
		m_strPresidentName = "";
		m_strPresidentGuildName = "";
		m_mapUserRank.clear();
		m_lstCityID.clear();
		m_nHpAdd = 0;
		m_nAtkAdd = 0;
	}
	~CLiteCountry()
	{
		m_lstCityID.clear();
		m_mapUserRank.clear();

		
	}
	int GetUserLastDayRank(int64 uid)
	{
//#ifdef WIN32
//		return 1;//临时代码，方便测试
//#endif
		map<int64,int>::iterator iter;
		iter = m_mapUserRank.find(uid);
		if(iter == m_mapUserRank.end())
		{
			return -1;
		}
		return iter->second;
	}
	void ClearUserRank()
	{
		if(m_ltUserRankGetTime<=0)
			return;
		m_ltUserRankGetTime = 0;
		m_mapUserRank.clear();
	}
	bool NeedGetCountryUserRank(time_t ltNow)
	{
		if(m_ltUserRankGetTime<=0)
		{
			return true;
		}
		time_t ltLast = m_ltUserRankGetTime;
		tm dayNow = *localtime(&ltNow);
		tm dayLast = *localtime(&ltLast);
		if(dayNow.tm_hour>=0 && dayNow.tm_hour<5)
		{
			return false;
		}
		if(dayNow.tm_yday!=dayLast.tm_yday || dayNow.tm_year!=dayLast.tm_year)
			return true;
		return false;
	}

	int m_nRegion;
	int m_nCountryID;
	int m_nUserCnt;
	int m_nCurRank;		//当前排行榜
	int m_nBoom;		//繁荣度
	int m_nGuildCnt;	//工会数量
	vector<int> m_lstCityID;	//城市列表

	int m_nLastDayRank;		//前一天国家排名
	int m_nLastDayBoom;		//前一天繁荣度

	int m_nAtkTargetCity;	//进攻目标城市
	int m_nDefTargetCity;	//防守目标城市

	time_t m_ltUserRankGetTime;		//玩家排行榜获得时间
	map<int64,int> m_mapUserRank;	//玩家排行
	int64 m_nPresidentUid ; //元首
	string m_strPresidentName ; //元首姓名
	string m_strPresidentGuildName ; //元首所属公会名

	int m_nHpAdd;
	int m_nAtkAdd;
};

class GameDataHandler;
class GameRgnDataHandler
{
public:
	GameRgnDataHandler(GameDataHandler* pDH,int nRegion);
	~GameRgnDataHandler();
	
	void Init();

	CLiteCountry* GetLiteCountryInfo(int nCountry,bool bCreateIfNotExit = false);
	void SetCountryUser(int nCountry,int nCnt);
	int  GetCountryUser(int nCountry);
	void AddCountryUser(int nCountry,int nCnt);		
	int  GetMinUserCountry();	//获得最少人数的国家
	int  GetMaxUserCountry();	//获得最多人数的国家
	int  GetCountryLastDayRank(int nCountry);//获得前一天国家排名	
	int  GetCountryBoom(int nCountry);	//获得国家繁荣度
	void ResetCountryRank();
	// 排行榜相关
	void SetFaouseHallRank(int64 uid, int nRank );
	int GetFaouseHallRank(int64 uid);
	void SetRankUpdateTime( time_t tTime ){ m_tRankFamoseHallTimer = tTime; }
	bool IsRankGetTimeByID(int nActivityId);

private:
	int m_nRegion;
	GameDataHandler* m_pGameDataHandler;

	//国家相关数据
	map<int,CLiteCountry*>		m_mapLiteCountry;
	//名人堂回馈排行榜，之后可将排行榜相关扩展以下map，将其他榜单并入其中
	map<int64,int>				m_mapFamouseGiftMap;
	time_t						m_tRankFamoseHallTimer; // 更新榜单时间

};
