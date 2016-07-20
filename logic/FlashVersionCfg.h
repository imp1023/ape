#pragma once
#include <map>
#include "../common/const_def.h"
using namespace std;


class FlashVersionCfg
{
	enum E_INIT_FILE
	{
		E_FILE_ID,
		E_FILE_MAINLOADER_VERSION,
		E_FILE_VERSION_KEY,
		E_FILE_CFG_VERSION,
	};


public:
	FlashVersionCfg(void);
	~FlashVersionCfg(void);

	static FlashVersionCfg& Instance()
	{
		static FlashVersionCfg inst;
		return inst;
	}

	bool			LoadTbl();
	void			ClearTbl();
	
	string			GetMainLoaderVersion(int nSrvID);
	string			GetCfgVersion(int nSrvID);
	string			GetVersionKey(int nSrvID);
protected:
	static const char*		szLevelFile;
	string			m_strDefMainLoaderVersion;
	string			m_strDefCfgVersion;
	string			m_strDefVersionKey;
	map<int,string> m_mapMainLoaderVersion;
	map<int,string> m_mapCfgVersion;
	map<int,string> m_mapVersionKey;

};

