#include "FlashVersionCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"

const char* FlashVersionCfg::szLevelFile		= "Config/FlashVersion.dat";

FlashVersionCfg::FlashVersionCfg()
{
	ClearTbl();
}

FlashVersionCfg::~FlashVersionCfg()
{
	ClearTbl();
}

bool FlashVersionCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szLevelFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		int nID = fileDBC.Search_Posistion(i, E_FILE_ID	)->iValue; 
		if(nID==-1)
		{
			m_strDefMainLoaderVersion	= fileDBC.Search_Posistion(i, E_FILE_MAINLOADER_VERSION	)->pString;
			m_strDefVersionKey			= fileDBC.Search_Posistion(i, E_FILE_VERSION_KEY	)->pString; 
			m_strDefCfgVersion			= fileDBC.Search_Posistion(i, E_FILE_CFG_VERSION	)->pString;
			int nPos=m_strDefMainLoaderVersion.find("_");
			if(nPos>=0)
			{
				m_strDefMainLoaderVersion = m_strDefMainLoaderVersion.substr(nPos+1);
			}
		
			nPos=m_strDefCfgVersion.find("_");
			if(nPos>=0)
			{
				m_strDefCfgVersion = m_strDefCfgVersion.substr(nPos+1);
			}
		
		}
		else
		{
			string strTmp = fileDBC.Search_Posistion(i, E_FILE_MAINLOADER_VERSION	)->pString;
			int nPos=strTmp.find("_");
			if(nPos>=0)
			{
				strTmp = strTmp.substr(nPos+1);
			}
			m_mapMainLoaderVersion[nID]	= strTmp;

			strTmp = fileDBC.Search_Posistion(i, E_FILE_VERSION_KEY	)->pString;
			m_mapVersionKey[nID] = strTmp;

			strTmp = fileDBC.Search_Posistion(i, E_FILE_CFG_VERSION	)->pString;
			nPos=strTmp.find("_");
			if(nPos>=0)
			{
				strTmp = strTmp.substr(nPos+1);
			}
			m_mapCfgVersion[nID] = strTmp;	


		}
	}
	printf("Load %s cnt:%d\n", szLevelFile, nRow);
	return true;

	return true;
}

void FlashVersionCfg::ClearTbl()
{
	m_strDefMainLoaderVersion	= "";
	m_strDefCfgVersion			= "";;
	m_strDefVersionKey				= "";
	m_mapMainLoaderVersion.clear();
	m_mapCfgVersion.clear();
	m_mapVersionKey.clear();
	
}

string FlashVersionCfg::GetVersionKey(int nSrvID)
{
	map<int,string>::iterator iter;
	iter = m_mapVersionKey.find(nSrvID);
	if(iter!=m_mapVersionKey.end())
	{
		return iter->second;
	}
	return m_strDefVersionKey;
}

string FlashVersionCfg::GetMainLoaderVersion(int nSrvID)
{
	map<int,string>::iterator iter;
	iter = m_mapMainLoaderVersion.find(nSrvID);
	if(iter!=m_mapMainLoaderVersion.end())
	{
		return iter->second;
	}
	return m_strDefMainLoaderVersion;


}
string FlashVersionCfg::GetCfgVersion(int nSrvID)
{
	map<int,string>::iterator iter;
	iter = m_mapCfgVersion.find(nSrvID);
	if(iter!=m_mapCfgVersion.end())
	{
		return iter->second;
	}
	return m_strDefCfgVersion;

}
