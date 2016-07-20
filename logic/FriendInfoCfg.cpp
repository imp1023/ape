#include "FriendInfoCfg.h"

FriendInfoCfg FriendInfoCfg::m_cfgFriendInfo;
const char* FriendInfoCfg::szConfigFile = "ServerCfg/FriendInfo.cfg";

FriendInfoCfg::FriendInfoCfg(void)
{
}

FriendInfoCfg::~FriendInfoCfg(void)
{
}

bool FriendInfoCfg::LoadTbl()
{
	try
	{
		m_configFriendInfo.readFile(szConfigFile);

		m_mapFriendInfoAddr.clear();
		m_mapFriendInfoPort.clear();

		m_nFriendDbNum = m_configFriendInfo.lookup("friendinfo.friendinfo_num");
		Setting &friendinfodb = m_configFriendInfo.lookup("friendinfo.friendinfodb");
		for (int i = 0; i < m_nFriendDbNum; i++ )
		{
			int id			= friendinfodb[i]["id"];
			string strAddr	= friendinfodb[i]["addr"];
			int port		= friendinfodb[i]["port"];
			m_mapFriendInfoAddr[id] = strAddr;
			m_mapFriendInfoPort[id]	= port;
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
