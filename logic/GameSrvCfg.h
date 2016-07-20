#ifndef _GAMESERVERCONFIG_H_
#define _GAMESERVERCONFIG_H_
#pragma once
#include <vector>
#include <string>
#include "../common/const_def.h"

#ifdef _WIN32
#pragma warning(disable: 4290)
#include <libconfig.hpp>
#else
#include <libconfig.h++>
#endif

using namespace std;
using namespace libconfig;

class GameSrvCfg
{
public:
	GameSrvCfg();
	~GameSrvCfg(void);

	bool	LoadTbl();

	int		SaveInterval() const { return m_nSaveInterval; }
	void	SaveInterval(int val) { m_nSaveInterval = val; }

	// Õ∑≈π¶ƒ‹
	int		FreeEnableFlag() const { return m_nFreeEnableFlag; }
	void	FreeEnableFlag(int val) { m_nFreeEnableFlag = val; }
	int		FreeUpdateTime() const { return m_nFreeUpdateTime; }
	void	FreeUpdateTime(int val) { m_nFreeUpdateTime = val; }
	int		FreeMaxUserCnt() const { return m_nFreeMaxUserCnt; }
	void	FreeMaxUserCnt(int val) { m_nFreeMaxUserCnt = val; }
	int		FreeMaxUserTime() const { return m_nFreeMaxUserTime; }
	void	FreeMaxUserTime(int val) { m_nFreeMaxUserTime = val; }
	int		FreeCnt() const { return m_nFreeCnt; }
	void	FreeCnt(int val) { m_nFreeCnt = val; }

	int		PlatFreeEnableFlag() const { return m_nPlatFreeEnableFlag; }
	int		PlatFreeUpdateTime() const { return m_nPlatFreeUpdateTime; }
	int		PlatFreeMaxUserCnt() const { return m_nPlatFreeMaxUserCnt; }
	int		PlatFreeMaxUserTime() const { return m_nPlatFreeMaxUserTime; }	//∫¡√Î
	int		PlatFreeCnt() const { return m_nPlatFreeCnt; }

	int		FriendMaxCnt() const { return m_nFriendMaxCnt; }
	void	FriendMaxCnt(int val) { m_nFriendMaxCnt = val; }

	int		PlatCacheEnable() const { return m_bPlatCacheEnable; }
	void	PlatCacheEnable(int val) { m_bPlatCacheEnable = val; }
	int		PlatCacheSaveInterval() const { return m_nPlatCacheSaveInterval; }
	void	PlatCacheSaveInterval(int val) { m_nPlatCacheSaveInterval = val; }
	int		PlatCacheSaveUpdateTime() const { return m_nPlatCacheSaveUpdateTime; }
	void	PlatCacheSaveUpdateTime(int val) { m_nPlatCacheSaveUpdateTime = val; }

	int		CountryCacheEnable() const { return m_bCountryCacheEnable; }
	void	CountryCacheEnable(int val) { m_bCountryCacheEnable = val; }
	int		CountryCacheSaveInterval() const { return m_nCountryCacheSaveInterval; }
	void	CountryCacheSaveInterval(int val) { m_nCountryCacheSaveInterval = val; }
	int		CountryCacheSaveUpdateTime() const { return m_nCountryCacheSaveUpdateTime; }
	void	CountryCacheSaveUpdateTime(int val) { m_nCountryCacheSaveUpdateTime = val; }

	int		RegionCacheEnable() const { return m_bRegionCacheEnable; }
	void	RegionCacheEnable(int val) { m_bRegionCacheEnable = val; }
	int		RegionCacheSaveInterval() const { return m_nRegionCacheSaveInterval; }
	void	RegionCacheSaveInterval(int val) { m_nRegionCacheSaveInterval = val; }
	int		RegionCacheSaveUpdateTime() const { return m_nRegionCacheSaveUpdateTime; }
	void	RegionCacheSaveUpdateTime(int val) { m_nRegionCacheSaveUpdateTime = val; }

	int		RankCacheEnable() const { return m_bRankCacheEnable; }
	void	RankCacheEnable(int val) { m_bRankCacheEnable = val; }
	int		RankCacheSaveInterval() const { return m_nRankCacheSaveInterval; }
	void	RankCacheSaveInterval(int val) { m_nRankCacheSaveInterval = val; }
	int		RankCacheSaveUpdateTime() const { return m_nRankCacheSaveUpdateTime; }
	void	RankCacheSaveUpdateTime(int val) { m_nRankCacheSaveUpdateTime = val; }


	int		CacheEnable() const { return m_bCacheEnable; }
	void	CacheEnable(int val) { m_bCacheEnable = val; }
	int		CacheFreeCnt() const { return m_nCacheFreeCnt; }
	void	CacheFreeCnt(int val) { m_nCacheFreeCnt = val; }
	int		CacheSaveInterval() const { return m_nCacheSaveInterval; }
	void	CacheSaveInterval(int val) { m_nCacheSaveInterval = val; }
	int		CacheSaveUpdateTime() const { return m_nCacheSaveUpdateTime; }
	void	CacheSaveUpdateTime(int val) { m_nCacheSaveUpdateTime = val; }
	int		CacheRemoveUpdateTime() const { return m_nCacheRemoveUpdateTime; }
	void	CacheRemoveUpdateTime(int val) { m_nCacheRemoveUpdateTime = val; }
	int		CacheRemoveTime() const { return m_nCacheRemoveTime; }
	void	CacheRemoveTime(int val) { m_nCacheRemoveTime = val; }

	int		FriendInfoEnable() const { return m_nFriendInfoEnable; }
	void	FriendInfoEnable(int val) { m_nFriendInfoEnable = val; }

	int		StarInfoEnable() const { return m_nStarInfoEnable; }
	void	StarInfoEnable(int val) { m_nStarInfoEnable = val; }

	int		GameHttpEnable() const { return m_nGameHttpEnable; }
	void	GameHttpEnable(int val) { m_nGameHttpEnable = val; }

	int		DefaultRegion() const { return m_nDefaultRegion; }
	void	DefaultRegion(int val) { m_nDefaultRegion = val; }

	int monitor_num() { return monitor_num_; };
	int monitorID( int index )	{ return monitor_id_[index]; }; 
	int monitor_gamed_Port(int id); 
	const string monitor_server_address(int id) ; 

	static GameSrvCfg&	Instance(){return m_cfgGameServer;}
protected:
	static const char*		szConfigFile;
	static GameSrvCfg	m_cfgGameServer;

	int		m_nDefaultRegion;

	int		m_nSaveInterval;

	int		m_nFreeEnableFlag;
	int		m_nFreeUpdateTime;
	int		m_nFreeMaxUserCnt;
	int		m_nFreeMaxUserTime;
	int		m_nFreeCnt;

	int		m_nPlatFreeEnableFlag;
	int		m_nPlatFreeUpdateTime;
	int		m_nPlatFreeMaxUserCnt;
	int		m_nPlatFreeMaxUserTime;
	int		m_nPlatFreeCnt;

	int		m_nFriendMaxCnt;

	int		m_bPlatCacheEnable;
	int		m_nPlatCacheSaveInterval;
	int		m_nPlatCacheSaveUpdateTime;

	int		m_bCountryCacheEnable;
	int		m_nCountryCacheSaveInterval;
	int		m_nCountryCacheSaveUpdateTime;

	int		m_bRegionCacheEnable;
	int		m_nRegionCacheSaveInterval;
	int		m_nRegionCacheSaveUpdateTime;


	int		m_bRankCacheEnable;
	int		m_nRankCacheSaveInterval;
	int		m_nRankCacheSaveUpdateTime;


	int		m_bCacheEnable;
	int		m_nCacheFreeCnt;
	int		m_nCacheSaveInterval;
	int		m_nCacheSaveUpdateTime;
	int		m_nCacheRemoveUpdateTime;
	int		m_nCacheRemoveTime;



	int		m_nStarInfoEnable;
	int		m_nGameHttpEnable;
	int		m_nFriendInfoEnable;
	Config	m_configSrv;

	//monitor srv
	int monitor_num_;
	vector<int> monitor_id_;
	map<int, string> monitor_server_address_;
	map<int, int> monitor_gamed_port_;

};

#endif


