#ifndef _FRIENDINFOCONFIG_H_
#define _FRIENDINFOCONFIG_H_
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

class FriendInfoCfg
{
public:
	FriendInfoCfg(void);
	~FriendInfoCfg(void);

	bool	LoadTbl();

	int		FriendDbNum() const { return m_nFriendDbNum; }
	string	FriendInfoAddr(int nId)  { return m_mapFriendInfoAddr[nId]; }
	int		FriendInfoPort(int nId)  { return m_mapFriendInfoPort[nId]; }

	static FriendInfoCfg&	Instance(){return m_cfgFriendInfo;}
protected:
	static const char*		szConfigFile;
	static FriendInfoCfg	m_cfgFriendInfo;

	int				 m_nFriendDbNum;
	map<int, string> m_mapFriendInfoAddr;
	map<int, int>    m_mapFriendInfoPort;
	
	Config	m_configFriendInfo;
};

#endif
