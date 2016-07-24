
#include <list>
#include "MemCacheServerHandler.h"
#include "../event/EventQueue.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/counter.h"
#include "../event/EventDefine.h"
#include "CountryEventHandler.h"
#include "../logic/GameSrvCfg.h"

#include "CountryDataHandler.h"
#include "../logic/Clock.h"
#include "../common/Ini.h"

MemCacheServerHandler::MSH_T* MemCacheServerHandler::g_pInst = NULL;
log4cxx::LoggerPtr MemCacheServerHandler::logger = log4cxx::Logger::getLogger("MemCacheServer");

MemCacheServerHandler::MemCacheServerHandler()
{
	m_bEnabled			= false;
	m_bInited			= false; 
	//m_eh				= NULL;
	m_SaveUpdate		= Clock::getCurrentSystemTime();

	//m_mapRevision.clear();
	pthread_mutex_init(&mutex_,NULL);
}

MemCacheServerHandler::~MemCacheServerHandler()
{
	pthread_mutex_destroy(&mutex_);
}

MemCacheServerHandler::MSH_T* MemCacheServerHandler::GetInst()
{
	if (g_pInst == NULL)
	{
		g_pInst = new MemCacheServerHandler();
	}
	return g_pInst;
}


bool MemCacheServerHandler::GetMemDBInfo(const string& strKey, std::string& dist)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

	TCRDB*  pConn   = g_pInst->GetDB(1);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_get_connect_null_error");
		return false;
	}

	
	int len  = 0;
	int klen = strKey.length();
	char* buffer = (char*)tcrdbget(pConn, strKey.c_str(), klen, &len);
	if (buffer == NULL)
	{
		int ecode = tcrdbecode(pConn);
		if (ecode != TTENOREC)
		{
			g_pInst->SetEnbale(false);
			LOG4CXX_ERROR(logger,"memcache_get_ecode_error");
		}
		return false;
	}
	std::string outbuf(buffer, len);
	dist = outbuf;
	free(buffer);	
	//g_pInst->GetCounter().increase("mem_get_user");
	return true;
}

bool MemCacheServerHandler::SaveMemDBInfo(string strKey,std::string& dist)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
	time_t time_first = Clock::getUTime();
	TCRDB*  pConn   = g_pInst->GetDB(1);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_put_connect_null_error");
		return false;
	}
	
	int nKeyLen = strKey.size();

	const char* buf = dist.c_str();
	int len = dist.length();
	if(!tcrdbput(pConn,strKey.c_str(),nKeyLen,buf,len))
	{
		int ecode = tcrdbecode(pConn);
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_update_put_error"<<tcrdberrmsg(ecode) << strKey);
		return false;
	}
	
	return true;
}

int MemCacheServerHandler::GetServerNumber()
{
	return m_nServerNum;
}

TCRDB* MemCacheServerHandler::GetDB(int id)
{
	TCRDB* pResult = (TCRDB*) (m_xConnList[id]);
	if (pResult == NULL)
	{
		pResult = tcrdbnew();
		std::string ip = m_xIpList[id];
		int port = m_xProtList[id];
		if (tcrdbopen(pResult, ip.c_str() ,port ))
		{
			m_xConnList[id] = pResult;
		}

	}
	return pResult;
}

void MemCacheServerHandler::LoadConfig()
{
	const char* filename = "Config/MemCacheServer.dat";
	DBCFile file(0);
	file.OpenFromTXT(filename);

	enum FS
	{
		FS_ID = 0,
		FS_IP = 1,
		FS_PROT = 2,
	} ;
	int cnt = file.GetRecordsNum();
	m_nServerNum = 0;
	for (int line = 0; line < cnt; line++)
	{

		int ind         = file.Search_Posistion(line, FS_ID)->iValue;
		std::string ip  = file.Search_Posistion(line, FS_IP)->pString;
		int prot        = file.Search_Posistion(line, FS_PROT)->iValue;
		m_xIpList[ind]  = ip;
		m_xProtList[ind] = prot;
		if (ind > m_nServerNum)
		{
			m_nServerNum = ind;
		}
	}
	m_bInited = true;
	SetEnbale(true);
}

void MemCacheServerHandler::SetEnbale(bool enable)
{
	m_bEnabled = enable;
	if (!enable)
	{
		Ini myini;
		if (myini.Open("MemErr_Country.ini") == false)
		{
			//return;
		}
		myini.Write("MemCache", "MemCacheError",1);
		struct tm  tm_val; 
		time_t rawtime = 0; 
		time_t time_now = time ( &rawtime ); 
		tm_val = *localtime ( &rawtime ); 
		char time_str[128] = {0};
		sprintf(time_str,"%d-%d-%d-%d:%d:%d",tm_val.tm_year + 1900,tm_val.tm_mon +1,tm_val.tm_mday,tm_val.tm_hour,tm_val.tm_min,tm_val.tm_sec);
		myini.Write("MemCache", "ErrDate",time_str);
		myini.Save();
	}
}

//void MemCacheServerHandler::SetEventHandler( CountryEventHandler* eh )
//{
//	m_eh = eh;
//}

bool MemCacheServerHandler::SafePushMemDB(int nKey,DB_C_GuildList* pDBGuildLst,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pDBGuildLst==NULL)
	{
		return false;
	}

	pDBGuildLst->set_ltsaverevision(revision_);
	map<int,DB_C_GuildList*> & userMap = g_pInst->GetGuildList();
	userMap[nKey] = pDBGuildLst;

	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,DB_C_UserList* pDBUserLst,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pDBUserLst==NULL)
	{
		return false;
	}

	pDBUserLst->set_ltsaverevision(revision_);
	map<int,DB_C_UserList*> & userMap = g_pInst->GetUserList();
	userMap[nKey] = pDBUserLst;

	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,CCountry* pCountry,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pCountry==NULL)
	{
		return false;
	}
	//	g_pInst->acquireLock();
	// 	if (!pCountry->Online())
	// 	{
	// 		return false;
	// 	}
	pCountry->m_dbCountry.set_ltsaverevision(revision_);
	map<int,CCountry*> & cityMap = g_pInst->GetCountryList();
	cityMap[nKey] = pCountry;
	//g_pInst->SetCountryRevision(platID,revision_);


	//	g_pInst->releaseLock();
	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,CCity* pCity,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pCity==NULL)
	{
		return false;
	}
//	g_pInst->acquireLock();
// 	if (!pCountry->Online())
// 	{
// 		return false;
// 	}
	pCity->m_dbCity.set_ltsaverevision(revision_);
	map<int,CCity*> & cityMap = g_pInst->GetCityList();
	cityMap[nKey] = pCity;
	//g_pInst->SetCountryRevision(platID,revision_);

	
//	g_pInst->releaseLock();
	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,DB_C_ResourceData* pDBResourceData,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pDBResourceData==NULL)
	{
		return false;
	}

	pDBResourceData->set_ltsaverevision(revision_);
	map<int,DB_C_ResourceData*> & resourceMap = g_pInst->GetResourceList();
	resourceMap[nKey] = pDBResourceData;

	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,CGvgCity* pCity,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pCity==NULL)
	{
		return false;
	}
	//	g_pInst->acquireLock();
	// 	if (!pCountry->Online())
	// 	{
	// 		return false;
	// 	}
	if ( nKey == 0 )
	{
		return false;
	}
	pCity->GetDBGvgCity()->set_ltsaverevision(revision_);
	map<int,CGvgCity*> & cityMap = g_pInst->GetGvgCityList();
	cityMap[nKey] = pCity;
	//g_pInst->SetCountryRevision(platID,revision_);


	//	g_pInst->releaseLock();
	return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,DB_C_WarGameData *pDBWarGame,time_t revision_)
{
    if (g_pInst == NULL || !g_pInst->CanUse()||pDBWarGame==NULL)
    {
        return false;
    }

    pDBWarGame->set_ltsaverevision(revision_);
    map<int,DB_C_WarGameData*> & resourceMap = g_pInst->GetWarGameList();
    resourceMap[nKey] = pDBWarGame;

    return true;
}

bool MemCacheServerHandler::SafePushMemDB(int nKey,DB_C_BossBtlMsg *pDBBossBtlMsg,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse()||pDBBossBtlMsg==NULL)
	{
		return false;
	}

	pDBBossBtlMsg->set_ltsaverevision(revision_);
	map<int,DB_C_BossBtlMsg*> & bossBtlMap = g_pInst->GetBossBtlMsgList();
	bossBtlMap[nKey] = pDBBossBtlMsg;

	return true;
}

bool MemCacheServerHandler::SaveAllMemDBData( int64 revision,bool urgent /* = false */ )
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

	if (!urgent && revision < g_pInst->GetSaveUpdateTime() + GameSrvCfg::Instance().CountryCacheSaveUpdateTime())
	{
		return false;
	}
	SaveAllCityMemDBData(revision,urgent);
	SaveAllCountryMemDBData(revision,urgent);
	SaveAllGuildLstMemDBData(revision,urgent);
	SaveAllUserLstMemDBData(revision,urgent);
	SaveAllResourceDBData(revision,urgent);
	SaveAllGvgCityMemDBData(revision,urgent);
    SaveAllWarGameMemDBData(revision,urgent);
	SaveAllBossBtlMsg(revision,urgent);
	g_pInst->SetSaveUpdateTime(revision);
	return true;
}


bool MemCacheServerHandler::SaveAllCityMemDBData(int64 revision,bool urgent)
{
	map<int,CCity*>::iterator itRes;
	map<int,CCity*> &citys = g_pInst->GetCityList();
	map<int,CCity*>::iterator iter = citys.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=citys.end())
	{
		map<int,CCity*>::iterator oiter = iter;
		iter ++;
		CCity *pCity = oiter->second;
		if (pCity == NULL||pCity->GetKey()<0)
		{
			continue;
		}
		int nKey = pCity->GetKey();
		DB_C_City* pDBCity = pCity->GetDBCity();
		int64 nRevision = pDBCity->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBCity->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"City_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				citys.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllCityMemDBData"<<nKey);
			}
		}
	}
	return true;
}

bool MemCacheServerHandler::SaveAllCountryMemDBData(int64 revision,bool urgent)
{
	map<int,CCountry*>::iterator itRes;
	map<int,CCountry*> &citys = g_pInst->GetCountryList();
	map<int,CCountry*>::iterator iter = citys.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=citys.end())
	{
		map<int,CCountry*>::iterator oiter = iter;
		iter ++;
		CCountry *pCountry = oiter->second;
		if (pCountry == NULL||pCountry->GetKey()<0)
		{
			continue;
		}
		int nKey = pCountry->GetKey();
		DB_C_Country* pDBCountry = pCountry->GetDBCountry();
		int64 nRevision = pDBCountry->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBCountry->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"Country_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				citys.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllCountryMemDBData"<<nKey);
			}
		}
	}
	return true;
}
bool MemCacheServerHandler::SaveAllGuildLstMemDBData(int64 revision,bool urgent /* = false */)
{
	map<int,DB_C_GuildList*>::iterator itRes;
	map<int,DB_C_GuildList*> &citys = g_pInst->GetGuildList();
	map<int,DB_C_GuildList*>::iterator iter = citys.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=citys.end())
	{
		map<int,DB_C_GuildList*>::iterator oiter = iter;
		iter ++;
		DB_C_GuildList *pDBLst = oiter->second;
		if (pDBLst == NULL||pDBLst->key()<0)
		{
			continue;
		}
		int nKey = pDBLst->key();
		int64 nRevision = pDBLst->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBLst->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"GuildLst_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				citys.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllGuildLstMemDBData"<<nKey);
			}
		}
	}
	return true;
}
bool MemCacheServerHandler::SaveAllUserLstMemDBData(int64 revision,bool urgent)
{
	map<int,DB_C_UserList*>::iterator itRes;
	map<int,DB_C_UserList*> &citys = g_pInst->GetUserList();
	map<int,DB_C_UserList*>::iterator iter = citys.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=citys.end())
	{
		map<int,DB_C_UserList*>::iterator oiter = iter;
		iter ++;
		DB_C_UserList *pDBLst = oiter->second;
		if (pDBLst == NULL||pDBLst->key()<0)
		{
			continue;
		}
		int nKey = pDBLst->key();
		int64 nRevision = pDBLst->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBLst->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"CUserLst_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				citys.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllUserLstMemDBData"<<nKey);
			}
		}
	}
	return true;
}

bool MemCacheServerHandler::SaveAllResourceDBData(int64 revision,bool urgent)
{
	map<int,DB_C_ResourceData*>::iterator itRes;
	map<int,DB_C_ResourceData*> &resources = g_pInst->GetResourceList();
	map<int,DB_C_ResourceData*>::iterator iter = resources.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=resources.end())
	{
		map<int,DB_C_ResourceData*>::iterator oiter = iter;
		iter ++;
		DB_C_ResourceData *pDBResource = oiter->second;
		if (pDBResource == NULL||pDBResource->key()<0)
		{
			continue;
		}
		int nKey = pDBResource->key();
		int64 nRevision = pDBResource->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBResource->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"CResourse_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				resources.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllUserLstMemDBData"<<nKey);
			}
		}
	}
	return true;
}

bool MemCacheServerHandler::SaveAllGvgCityMemDBData(int64 revision,bool urgent)
{
	map<int,CGvgCity*>::iterator itRes;
	map<int,CGvgCity*> &citys = g_pInst->GetGvgCityList();
	map<int,CGvgCity*>::iterator iter = citys.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=citys.end())
	{
		map<int,CGvgCity*>::iterator oiter = iter;
		iter ++;
		CGvgCity *pCity = oiter->second;
		if (pCity == NULL)
		{
			continue;
		}
		
		DB_C_GvgCity* pDBCity = pCity->GetDBGvgCity();
		if ( pDBCity == NULL || pDBCity->key()<0 )
		{
			continue;
		}
		int nKey = pDBCity->key();
		int64 nRevision = pDBCity->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBCity->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"GvgCity_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				citys.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllGvgCityMemDBData"<<nKey);
			}
		}
	}
	return true;
}

bool MemCacheServerHandler::SaveAllWarGameMemDBData(int64 revision,bool urgent/* = false*/)
{
    map<int,DB_C_WarGameData*>::iterator itRes;
    map<int,DB_C_WarGameData*> &wargames = g_pInst->GetWarGameList();
    map<int,DB_C_WarGameData*>::iterator iter = wargames.begin();

    int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
    while(iter!=wargames.end())
    {
        map<int,DB_C_WarGameData*>::iterator oiter = iter;
        iter ++;
        DB_C_WarGameData *pDBWarGame = oiter->second;
        if (pDBWarGame == NULL||pDBWarGame->key()<0)
        {
            continue;
        }
        int nKey = pDBWarGame->key();
        int64 nRevision = pDBWarGame->ltsaverevision();

        if( urgent || nRevision < revision - save_interval)
        {
            string plat_info;
            pDBWarGame->SerializeToString(&plat_info);

            char szKey[256];
            sprintf(szKey,"CWarGame_%d",nKey);
            bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
            if (sucess)
            {
                wargames.erase(oiter);
                LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllWarGameMemDBData"<<nKey);
            }
        }
    }
    return true;
}

bool MemCacheServerHandler::SaveAllBossBtlMsg(int64 revision,bool urgent/* = false*/)
{
	map<int,DB_C_BossBtlMsg*>::iterator itRes;
	map<int,DB_C_BossBtlMsg*> &bossBltMsgs = g_pInst->GetBossBtlMsgList();
	map<int,DB_C_BossBtlMsg*>::iterator iter = bossBltMsgs.begin();

	int save_interval = GameSrvCfg::Instance().CountryCacheSaveInterval();		//10s
	while(iter!=bossBltMsgs.end())
	{
		map<int,DB_C_BossBtlMsg*>::iterator oiter = iter;
		iter ++;
		DB_C_BossBtlMsg *pDBBossBtlMsg = oiter->second;
		if (pDBBossBtlMsg == NULL||pDBBossBtlMsg->key()<0)
		{
			continue;
		}
		int nKey = pDBBossBtlMsg->key();
		int64 nRevision = pDBBossBtlMsg->ltsaverevision();

		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBBossBtlMsg->SerializeToString(&plat_info);

			char szKey[256];
			sprintf(szKey,"CBossBattle_%d",nKey);
			bool sucess = MemCacheServerHandler::SaveMemDBInfo(szKey,plat_info);
			if (sucess)
			{
				bossBltMsgs.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllWarGameMemDBData"<<nKey);
			}
		}
	}
	return true;
}