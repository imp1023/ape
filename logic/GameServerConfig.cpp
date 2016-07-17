
#include "GameServerConfig.h"

GameServerConfig GameServerConfig::m_cfgGameServer;
const char* GameServerConfig::szConfigFile = "ServerCfg/gameserver.cfg";
GameServerConfig::GameServerConfig()
{
	if (!loadFromFile()) {
		printf("WARNING! read serverconfig file failed.\n");
		exit(1);
	}
}

GameServerConfig::~GameServerConfig(void)
{

}

bool GameServerConfig::loadFromFile()
{
	try
	{
		m_configSrv.readFile(szConfigFile);

		m_nSaveInterval			= m_configSrv.lookup("save_interval");
		m_nErrorSaveEnable		= m_configSrv.lookup("EnableErrorSave");

        m_nEventTimerInteral    = m_configSrv.lookup("Event.TimerInterval");

		m_nFreeEnableFlag		= m_configSrv.lookup("UserFree.EnableFlag");
		m_nFreeUpdateTime		= m_configSrv.lookup("UserFree.UpdateTime");
		m_nFreeMaxUserCnt		= m_configSrv.lookup("UserFree.MaxUserCnt");
		m_nFreeMaxUserTime		= m_configSrv.lookup("UserFree.MaxUserTime");
		m_nFreeCnt				= m_configSrv.lookup("UserFree.FreeCnt");

		m_bCacheEnable			= m_configSrv.lookup("MemCache.EnableFlag");
		m_nCacheFreeCnt			= m_configSrv.lookup("MemCache.FreeCnt");
		m_nCacheSaveInterval	= m_configSrv.lookup("MemCache.SaveInterval");
		m_nCacheSaveUpdateTime	= m_configSrv.lookup("MemCache.SaveUpdateTime");
		m_nCacheRemoveUpdateTime= m_configSrv.lookup("MemCache.RemoveUpdateTime");
		m_nCacheRemoveTime		= m_configSrv.lookup("MemCache.RemoveTime");

        m_nBackupDBEnable       = m_configSrv.lookup("Database.EnableBackup");
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

