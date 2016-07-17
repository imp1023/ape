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

class GameServerConfig
{
public:
	GameServerConfig();
	~GameServerConfig(void);

	bool	loadFromFile();

	int		SaveInterval() const { return m_nSaveInterval; }
	void	SaveInterval(int val) { m_nSaveInterval = val; }

	int		ErrorSaveEnable() const { return m_nErrorSaveEnable; }
	void	ErrorSaveEnable(int val) { m_nErrorSaveEnable = val; }

    //Event
    int     EventTimerInteral() const {return m_nEventTimerInteral; }

	//ÊÍ·Å¹¦ÄÜ
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

    int     BackupDBEnable() const { return m_nBackupDBEnable; }
    void    BackupDBEnable(int val) { m_nBackupDBEnable = val; }

	static GameServerConfig&	Instance(){return m_cfgGameServer;}
protected:
	static const char*		szConfigFile;
	static GameServerConfig	m_cfgGameServer;

	int		m_nSaveInterval;
	int		m_nErrorSaveEnable;

    int     m_nEventTimerInteral;
	
	int		m_nFreeEnableFlag;
	int		m_nFreeUpdateTime;
	int		m_nFreeMaxUserCnt;
	int		m_nFreeMaxUserTime;
	int		m_nFreeCnt;

	int		m_bCacheEnable;
	int		m_nCacheFreeCnt;
	int		m_nCacheSaveInterval;
	int		m_nCacheSaveUpdateTime;
	int		m_nCacheRemoveUpdateTime;
	int		m_nCacheRemoveTime;

    int     m_nBackupDBEnable;
    
	Config	m_configSrv;
};

#endif


