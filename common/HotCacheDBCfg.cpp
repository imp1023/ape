#include "HotCacheDBCfg.h"


const char* HotCacheDBCfg::szConfigFile = "ServerCfg/HotCacheDB.cfg";

HotCacheDBCfg::HotCacheDBCfg(void)
{
}

HotCacheDBCfg::~HotCacheDBCfg(void)
{
}

bool HotCacheDBCfg::LoadTbl()
{
	try
	{
		m_configHotCacheDB.readFile(szConfigFile);

		m_mapGameDBAddr.clear();
		m_mapGameDBPort.clear();
		m_mapPlatDBAddr.clear();
		m_mapPlatDBPort.clear();


		m_nHotCacheDBNum= m_configHotCacheDB.lookup("HotCacheDB.db_num");
		m_bEnableWrite	= int(m_configHotCacheDB.lookup("HotCacheDB.enable_write")) != 0;
		m_bEnableRead	= int(m_configHotCacheDB.lookup("HotCacheDB.enable_read")) != 0;
		m_bEnableReadNPC= int(m_configHotCacheDB.lookup("HotCacheDB.enable_readnpc")) != 0;
		Setting &platDB = m_configHotCacheDB.lookup("HotCacheDB.PlatDB");
		for (int i = 0; i < m_nHotCacheDBNum; i++ )
		{
			int id			= platDB[i]["id"];
			string strAddr	= platDB[i]["addr"];
			int port		= platDB[i]["port"];
			m_mapPlatDBAddr[id] = strAddr;
			m_mapPlatDBPort[id]	= port;
		}

		Setting &gameDB = m_configHotCacheDB.lookup("HotCacheDB.UserDB");
		for (int i = 0; i < m_nHotCacheDBNum; i++ )
		{
			int id			= gameDB[i]["id"];
			string strAddr	= gameDB[i]["addr"];
			int port		= gameDB[i]["port"];
			m_mapGameDBAddr[id] = strAddr;
			m_mapGameDBPort[id]	= port;
		}

		m_nHotCacheCountryDBNum = m_configHotCacheDB.lookup("HotCacheDB.countryDB_num");
		m_bEnableCountryDB		= int(m_configHotCacheDB.lookup("HotCacheDB.enable_country")) != 0;
		Setting &countryDB = m_configHotCacheDB.lookup("HotCacheDB.CountryDB");
		for (int i = 0; i < m_nHotCacheCountryDBNum; i++ )
		{
			int id			= countryDB[i]["id"];
			string strAddr	= countryDB[i]["addr"];
			int port		= countryDB[i]["port"];
			m_mapCountryDBAddr[id] = strAddr;
			m_mapCountryDBPort[id]	= port;
		}

		m_nHotCacheRegionDBNum = m_configHotCacheDB.lookup("HotCacheDB.regionDB_num");
		m_bEnableRegionDB		= int(m_configHotCacheDB.lookup("HotCacheDB.enable_region")) != 0;
		Setting &regionDB = m_configHotCacheDB.lookup("HotCacheDB.RegionDB");
		for (int i = 0; i < m_nHotCacheRegionDBNum; i++ )
		{
			int id			= regionDB[i]["id"];
			string strAddr	= regionDB[i]["addr"];
			int port		= regionDB[i]["port"];
			m_mapRegionDBAddr[id] = strAddr;
			m_mapRegionDBPort[id]	= port;
		}

		m_nHotCacheRankDBNum = m_configHotCacheDB.lookup("HotCacheDB.rankDB_num");
		m_bEnableRankDB		= int(m_configHotCacheDB.lookup("HotCacheDB.enable_rank")) != 0;
		Setting &rankDB = m_configHotCacheDB.lookup("HotCacheDB.RankDB");
		for (int i = 0; i < m_nHotCacheRegionDBNum; i++ )
		{
			int id			= rankDB[i]["id"];
			string strAddr	= rankDB[i]["addr"];
			int port		= rankDB[i]["port"];
			m_mapRankDBAddr[id] = strAddr;
			m_mapRankDBPort[id]	= port;
		}
	}
	catch (ParseException ex)
	{
		printf("Parsing config file %s failed.\n", szConfigFile);
		return false;
	}
	catch (FileIOException ex) 
	{
		printf("Read config file %s failed. IOExcetpion.\n", szConfigFile);
		return false;
	}
	catch (SettingNotFoundException ex) 
	{
		printf("Read config file %s failed. Setting \"%s\" not found.\n", szConfigFile, ex.getPath());
		return false;
	}
	return true;
}
