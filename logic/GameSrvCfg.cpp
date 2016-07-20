#include "GameSrvCfg.h"
#include "../common/Ini.h"
GameSrvCfg GameSrvCfg::m_cfgGameServer;
const char* GameSrvCfg::szConfigFile = "ServerCfg/gameserver.cfg";
GameSrvCfg::GameSrvCfg()
{

}

GameSrvCfg::~GameSrvCfg(void)
{

}

bool GameSrvCfg::LoadTbl()
{
	try
	{
		m_configSrv.readFile(szConfigFile);

		m_nDefaultRegion		= m_configSrv.lookup("Region.DefaultRegion"); 

		m_nSaveInterval			= m_configSrv.lookup("save_interval");

		m_nFreeEnableFlag		= m_configSrv.lookup("UserFree.EnableFlag");
		m_nFreeUpdateTime		= m_configSrv.lookup("UserFree.UpdateTime");
		m_nFreeMaxUserCnt		= m_configSrv.lookup("UserFree.MaxUserCnt");
		m_nFreeMaxUserTime		= m_configSrv.lookup("UserFree.MaxUserTime");
		m_nFreeMaxUserTime	   *= 1000;
		m_nFreeCnt				= m_configSrv.lookup("UserFree.FreeCnt");

		m_nPlatFreeEnableFlag	= m_configSrv.lookup("PlatFree.EnableFlag");
		m_nPlatFreeUpdateTime	= m_configSrv.lookup("PlatFree.UpdateTime");
		m_nPlatFreeMaxUserCnt	= m_configSrv.lookup("PlatFree.MaxCnt");
		m_nPlatFreeMaxUserTime	= m_configSrv.lookup("PlatFree.MaxTime");
		m_nPlatFreeMaxUserTime *= 1000;
		m_nPlatFreeCnt			= m_configSrv.lookup("PlatFree.FreeCnt");

		m_nFriendMaxCnt			= m_configSrv.lookup("Friend.FriendMaxCnt");

		m_bCacheEnable          = m_configSrv.lookup("MemCache.EnableFlag");
		Ini memini;
		int nMemCacheErr = 0;
		if (memini.Open("MemErr.ini"))
		{
			nMemCacheErr = memini.Readint("MemCache", "MemCacheError");
		}

		if (nMemCacheErr > 0)
		{
			m_bCacheEnable = false;
		}
		m_nCacheFreeCnt			= m_configSrv.lookup("MemCache.EnableFlag");
		m_nCacheFreeCnt			= m_configSrv.lookup("MemCache.FreeCnt");
		m_nCacheSaveInterval	= m_configSrv.lookup("MemCache.SaveInterval");
		m_nCacheSaveUpdateTime	= m_configSrv.lookup("MemCache.SaveUpdateTime");
		m_nCacheRemoveUpdateTime= m_configSrv.lookup("MemCache.RemoveUpdateTime");
		m_nCacheRemoveTime		= m_configSrv.lookup("MemCache.RemoveTime");

		m_bPlatCacheEnable		= m_configSrv.lookup("PlatMemCache.EnableFlag");
		Ini memini1;
		nMemCacheErr = 0;
		if (memini1.Open("MemErr_Plat.ini"))
		{
			nMemCacheErr = memini1.Readint("MemCache", "MemCacheError");
		}
		m_nPlatCacheSaveInterval	= m_configSrv.lookup("PlatMemCache.SaveInterval");
		m_nPlatCacheSaveUpdateTime	= m_configSrv.lookup("PlatMemCache.SaveUpdateTime");

		if (nMemCacheErr > 0)
		{
			m_bPlatCacheEnable = false;
		}		

		m_bCountryCacheEnable		= m_configSrv.lookup("CountryMemCache.EnableFlag");
		Ini memini2;
		nMemCacheErr = 0;
		if (memini2.Open("MemErr_Country.ini"))
		{
			nMemCacheErr = memini2.Readint("MemCache", "MemCacheError");
		}
		m_nCountryCacheSaveInterval	= m_configSrv.lookup("CountryMemCache.SaveInterval");
		m_nCountryCacheSaveUpdateTime	= m_configSrv.lookup("CountryMemCache.SaveUpdateTime");

		m_bRegionCacheEnable		= m_configSrv.lookup("RegionMemCache.EnableFlag");
		Ini memini3;
		nMemCacheErr = 0;
		if (memini3.Open("MemErr_Region.ini"))
		{
			nMemCacheErr = memini3.Readint("MemCache", "MemCacheError");
		}
		m_nRegionCacheSaveInterval	= m_configSrv.lookup("RegionMemCache.SaveInterval");
		m_nRegionCacheSaveUpdateTime	= m_configSrv.lookup("RegionMemCache.SaveUpdateTime");


		//rank
		m_bRankCacheEnable		= m_configSrv.lookup("RankMemCache.EnableFlag");
		Ini memini4;
		nMemCacheErr = 0;
		if (memini4.Open("MemErr_Rank.ini"))
		{
			nMemCacheErr = memini4.Readint("MemCache", "MemCacheError");
		}
		m_nRankCacheSaveInterval	= m_configSrv.lookup("RankMemCache.SaveInterval");
		m_nRankCacheSaveUpdateTime	= m_configSrv.lookup("RankMemCache.SaveUpdateTime");


		m_nFriendInfoEnable		= m_configSrv.lookup("FriendInfo.EnableFlag");

		m_nStarInfoEnable		= m_configSrv.lookup("UserLiteInfo.EnableStar");

		m_nGameHttpEnable		= m_configSrv.lookup("GameHttp.EnableFlag");

		//monitor srv
		monitor_id_.clear();
		monitor_server_address_.clear();
		monitor_gamed_port_.clear();
		monitor_num_ = m_configSrv.lookup("monitorsrv_num");
		Setting &monitorsrv = m_configSrv.lookup("monitor");
		for ( int i = 0; i < monitor_num_; i++ )
		{
			int id = monitorsrv[i]["id"];
			monitor_id_.push_back( id );
			monitor_server_address_.insert( make_pair( id, (const char*) monitorsrv[i]["addr"]));
			monitor_gamed_port_.insert( make_pair( id, (int) monitorsrv[i]["port"]));
		}
	}
	catch (ParseException ex)
	{
		printf("Parsing config file %s failed.\n", szConfigFile);
		return false;
	}
	catch (FileIOException ex) {
		printf("Read config file %s failed. IOExcetpion.\n", szConfigFile);
		return false;
	}
	catch (SettingNotFoundException ex) {
		printf("Read config file %s failed. Setting \"%s\" not found.\n", szConfigFile, ex.getPath());
		return false;
	}
	return true;
}


int GameSrvCfg::monitor_gamed_Port(int id)
{
	map<int, int>::const_iterator iter = monitor_gamed_port_.find(id);
	if (iter==monitor_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

const string GameSrvCfg::monitor_server_address(int id)
{
	map<int, string>::const_iterator iter = monitor_server_address_.find(id);
	if (iter==monitor_server_address_.end()) 
		return "";
	return iter->second; 
}
