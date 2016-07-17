#include <string>
#include <stdio.h>
#include "../common/Clock.h"
#include "../logic/GameServerConfig.h"
#include "../logic/User.h"
#include "GameCacheModule.h"
#include "GameDataHandler.h"
#include "GameEventHandler.h"

GameCacheModule::GameCacheModule(void)
: m_pEventHandler(NULL)
, m_bEnable(true)
, m_pDbCache(NULL)
, m_timeUpdataRemove(0)
, m_timeUpdateSave(0)
{
	m_logger = log4cxx::Logger::getLogger("GameCacheModule");
}

GameCacheModule::~GameCacheModule(void)
{
	if (m_pDbCache != NULL)
	{
		tcrdbclose((TCRDB*)m_pDbCache);
		tcrdbdel((TCRDB*)m_pDbCache);
		m_pDbCache = NULL;
	}
}

void GameCacheModule::WriteError(string strError)
{
	FILE *fileCache = fopen("cacheerror.log", "w");
	if (fileCache != NULL)
	{
		fprintf(fileCache, "%s",strError.c_str());
		fclose(fileCache);
	}
}

bool GameCacheModule::ReadError()
{
	FILE *fileCache = fopen("cacheerror.log", "r");
	if (fileCache != NULL)
	{
		fclose(fileCache);
		return true;
	}
	else
	{
		return false;
	}
}

void GameCacheModule::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

bool GameCacheModule::GetUserInfo(int64 uid, string& strUserInfo)
{
	if (!CanUse())
	{
		return false;
	}
	

	TCRDB*  pDbCache   = GetDBCache();
	if (pDbCache == NULL)
	{
		SetEnable(false);
		return false;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);

	int klen = 16;
	int len  = 0;
	char* buffer = (char*) tcrdbget(pDbCache, suid, klen, &len);
	if (buffer == NULL)
	{
		int ecode = tcrdbecode(pDbCache);
		if (ecode != TTENOREC)
		{
			tcrdbclose(pDbCache);
			tcrdbdel(pDbCache);
			m_pDbCache = NULL;
			SetEnable(false);
		}
		return false;
	}
	strUserInfo.assign(buffer,len);
	free(buffer);
	return true;
}

bool GameCacheModule::PutUserInfo(int64 uid, string& strUserInfo)
{
	if (!CanUse())
	{
		return false;
	}

	TCRDB*  pDbCache   = GetDBCache();
	if (pDbCache == NULL)
	{
		SetEnable(false);
		return false;
	}

	const char* buf = strUserInfo.c_str();
	int         len = strUserInfo.length();
	char suid[32];
	sprintf(suid, "%016llX", uid);
	int klen = 16;
	if (!tcrdbput(pDbCache, suid, klen, buf, len))
	{
		int ecode = tcrdbecode(pDbCache);
		string strError = tcrdberrmsg(ecode);
		WriteError(strError);
		tcrdbclose(pDbCache);
		tcrdbdel(pDbCache);
		m_pDbCache = NULL;
		SetEnable(false);
		return false;
	}

	return true;
}

bool GameCacheModule::RemoveUserInfo(int64 uid)
{
	if (!CanUse())
	{
		return false;
	}

	TCRDB*  pDbCache   = GetDBCache();
	if (pDbCache == NULL)
	{
		SetEnable(false);
		return false;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	int klen = 16;
	if (!tcrdbout(pDbCache, suid, klen))
	{
		tcrdbecode(pDbCache);
		tcrdbclose(pDbCache);
		tcrdbdel(pDbCache);
		m_pDbCache = NULL;
		return false;
	}
	return true;
}

bool GameCacheModule::HasSaveUser(int64 uid)
{
	if (!CanUse())
	{
		return false;
	}

	map<int64, bool>::iterator iterFind = m_mapUserSave.find(uid);
	if (iterFind == m_mapUserSave.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void GameCacheModule::AddRemoveUser(int64 uid)
{
	if (!CanUse())
	{
		return;
	}

	m_mapUserRemove[uid] = true;
}

void GameCacheModule::AddSaveUser(int64 uid)
{
	if (!CanUse())
	{
		return;
	}

	m_mapUserSave[uid] = true;
}

bool GameCacheModule::CanRemoveUser(int64 uid)
{
	const hash_map<int64, User*> & mapUsers = m_pEventHandler->getDataHandler()->getUsers();
	const map<int64, User*> &mapDirtyUsers = m_pEventHandler->getDataHandler()->getDirtyUsers();

	hash_map<int64, User*>::const_iterator iterUser = mapUsers.find(uid);
	if (iterUser == mapUsers.end())
	{
		return true;
	}
	
	User * pUser = (User*)iterUser->second;
	if (pUser == NULL)
	{
		return true;
	}
	
	if (pUser->Online())
	{
		return false;
	}

	map<int64, User*>::const_iterator iter_dirty = mapDirtyUsers.find(uid);
	if (iter_dirty != mapDirtyUsers.end())
	{
		return false;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if (timeNow < pUser->getMemRevision() + GameServerConfig::Instance().CacheRemoveTime() *1000)
	{
		return false;
	}
	
	return true;
}

bool GameCacheModule::UpdateRemoveUser()
{
	if (m_pEventHandler == NULL)
	{
		return false;
	}

	if (!CanUse())
	{
		return false;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if (timeNow - m_timeUpdataRemove < GameServerConfig::Instance().CacheRemoveUpdateTime())
	{
		return false;
	}

	int free_cnt(0);

	map<int64,bool>::iterator iter = m_mapUserRemove.begin();
	while(iter != m_mapUserRemove.end())
	{
		map<int64, bool>::iterator iterTemp = iter;
		iter ++;
		int64 uid = iterTemp->first;
		
		if (CanRemoveUser(uid))
		{
			RemoveUserInfo(uid);
			m_mapUserRemove.erase(iterTemp);
			free_cnt++;
		}		

		if (free_cnt >= GameServerConfig::Instance().CacheFreeCnt())
		{
			break;
		}

		time_t time_last = Clock::getCurrentSystemTime();
		if (time_last - timeNow > 20)
		{
			break;
		}
	}

	m_timeUpdataRemove = timeNow;
	return true;
}

bool GameCacheModule::UpdateSaveUser()
{
	if (m_pEventHandler == NULL)
	{
		return false;
	}

	if (!CanUse())
	{
		return false;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if (timeNow < m_timeUpdateSave + GameServerConfig::Instance().CacheSaveUpdateTime())
	{
		return false;
	}

	const hash_map<int64, User*> & mapUsers = m_pEventHandler->getDataHandler()->getUsers();
	map<int64, bool>::iterator iter = m_mapUserSave.begin();
	int save_interval = GameServerConfig::Instance().CacheSaveInterval();
	while(iter!=m_mapUserSave.end())
	{
		map<int64, bool>::iterator oiter = iter;
		iter ++;

		hash_map<int64, User*>::const_iterator iterUser = mapUsers.find((int64)oiter->first);
		if (iterUser == mapUsers.end())
		{
			m_mapUserSave.erase(oiter);
			continue;
		}

		User *pUser = (User*)iterUser->second;
		if (pUser == NULL)
		{
			m_mapUserSave.erase(oiter);
			continue;
		}

		if(pUser->getMemRevision() < timeNow - save_interval)
		{
			string strUserInfo;
			pUser->GetDbUser().SerializeToString(&strUserInfo);
			bool sucess = PutUserInfo(pUser->uid(), strUserInfo);
			if (sucess)
			{
				AddRemoveUser(pUser->uid());
				m_mapUserSave.erase(oiter);
			}
		}
	}

	m_timeUpdateSave = timeNow;
	return true;
}

const string	CACHEIP = "/tmp/cache";
const int		CCHEPORT = 0;
TCRDB*	GameCacheModule::GetDBCache()
{
	TCRDB* pDbCache = (TCRDB*) m_pDbCache;
	if (pDbCache == NULL)
	{
		pDbCache = tcrdbnew();
	
		if (tcrdbopen(pDbCache, CACHEIP.c_str(), CCHEPORT))
		{
			m_pDbCache= pDbCache;
		}
		else
		{
			tcrdbdel(pDbCache);
			pDbCache = NULL;
		}

	}
	return pDbCache;
}

bool GameCacheModule::CanUse()
{
	int bInited = GameServerConfig::Instance().CacheEnable();
	bool bError = ReadError();
	return m_bEnable && bInited && m_pEventHandler != NULL && !bError;
}
