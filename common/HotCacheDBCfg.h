#pragma once

#include <map>
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

class HotCacheDBCfg
{
public:
	HotCacheDBCfg(void);
	~HotCacheDBCfg(void);

	bool	LoadTbl();

	int		GetHotCacheDBNum() const { return m_nHotCacheDBNum; }
	string	GameMemDBAddr(int nId)  { return m_mapGameDBAddr[nId]; }
	int		GameMemDBPort(int nId)  { return m_mapGameDBPort[nId]; }

	string	PlatMemDBAddr(int nId)  { return m_mapPlatDBAddr[nId]; }
	int		PlatMemDBPort(int nId)  { return m_mapPlatDBPort[nId]; }

	bool	EnableWrite(){return m_bEnableWrite;}
	bool	EnableRead() {return m_bEnableRead;}
	bool	EnableReadNPC(){return m_bEnableReadNPC;}
	
	bool	EnableCountryDB(){return m_bEnableCountryDB;}
	string	CountryMemDBAddr(int nID)  { return m_mapCountryDBAddr[nID]; }
	int		CountryMemDBPort(int nID)  { return m_mapCountryDBPort[nID]; }

	bool	EnableRegionDB(){return m_bEnableRegionDB;}
	string	RegionMemDBAddr(int nID)  { return m_mapRegionDBAddr[nID]; }
	int		RegionMemDBPort(int nID)  { return m_mapRegionDBPort[nID]; }

	bool	EnableRankDB(){return m_bEnableRankDB;}
	string	RankMemDBAddr(int nID)  { return m_mapRankDBAddr[nID]; }
	int		RankMemDBPort(int nID)  { return m_mapRankDBPort[nID]; }

	static HotCacheDBCfg& Instance()
	{
		static HotCacheDBCfg cfg;
		return cfg;
	}
protected:
	static const char*		szConfigFile;

	int				 m_nHotCacheDBNum;
	map<int, string> m_mapGameDBAddr;
	map<int, int>    m_mapGameDBPort;
	map<int, string> m_mapPlatDBAddr;
	map<int, int>    m_mapPlatDBPort;
	bool			 m_bEnableWrite;
	bool			 m_bEnableRead;
	bool			 m_bEnableReadNPC;

	int				 m_nHotCacheCountryDBNum;
	map<int, string> m_mapCountryDBAddr;
	map<int, int>    m_mapCountryDBPort;
	bool			 m_bEnableCountryDB;	

	int				 m_nHotCacheRegionDBNum;
	map<int, string> m_mapRegionDBAddr;
	map<int, int>    m_mapRegionDBPort;
	bool			 m_bEnableRegionDB;	

	int				 m_nHotCacheRankDBNum;
	map<int, string> m_mapRankDBAddr;
	map<int, int>    m_mapRankDBPort;
	bool			 m_bEnableRankDB;	

	Config	m_configHotCacheDB;
};
