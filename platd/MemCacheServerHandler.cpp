

#include <list>
#include "MemCacheServerHandler.h"
#include "../event/EventQueue.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/counter.h"
#include "../event/EventDefine.h"
#include "PlatEventHandler.h"
#include "../logic/GameSrvCfg.h"

#include "PlatDataHandler.h"
#include "../common/Clock.h"
#include "../common/Ini.h"

MemCacheServerHandler::MSH_T* MemCacheServerHandler::g_pInst = NULL;
log4cxx::LoggerPtr MemCacheServerHandler::logger = log4cxx::Logger::getLogger("MemCacheServer");

MemCacheServerHandler::MemCacheServerHandler()
{
	m_bEnabled			= false;
	m_bInited			= false; 
	m_eh				= NULL;
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

int MemCacheServerHandler::GetServerId(const string& platID)
{
	int hash = getPlatidHash(platID);
	return hash % GetServerNumber() + 1;
}

bool MemCacheServerHandler::GetPlatInfo(const string& platID, std::string& dist)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

// 	if (uid != 2928562105553271)
// 	{
// 		return false;
// 	}

	int     dbid    = g_pInst->GetServerId(platID);
	TCRDB*  pConn   = g_pInst->GetDB(dbid);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_get_connect_null_error");
		return false;
	}

	
	int len  = 0;
	int klen = platID.length();
	char* buffer = (char*)tcrdbget(pConn, platID.c_str(), klen, &len);
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

bool MemCacheServerHandler::UpdatePlatInfo(const string& platID,std::string& dist)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
	time_t time_first = Clock::getUTime();
	int     dbid    = g_pInst->GetServerId(platID);
	TCRDB*  pConn   = g_pInst->GetDB(dbid);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_put_connect_null_error");
		return false;
	}

	const char* buf = dist.c_str();
	int len = dist.length();
	if(!tcrdbput(pConn, platID.c_str(), platID.size(), buf, len))
	{
		int ecode = tcrdbecode(pConn);
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_update_put_error"<<tcrdberrmsg(ecode) << platID);
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
		if (myini.Open("MemErr_Plat.ini") == false)
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

void MemCacheServerHandler::SetEventHandler( PlatEventHandler* eh )
{
	m_eh = eh;
}


bool MemCacheServerHandler::SafePushPlat(const string& platID,DB_Plat * pPlat,time_t revision_)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
//	g_pInst->acquireLock();
// 	if (!pPlat->Online())
// 	{
// 		return false;
// 	}
	
	map<string,DB_Plat*> & plat_map = g_pInst->GetPlatList();
	pPlat->set_ltmemrevision(revision_);
	plat_map[platID] = pPlat;
	//g_pInst->SetPlatRevision(platID,revision_);

	
//	g_pInst->releaseLock();
	return true;
}


bool MemCacheServerHandler::SaveAllPlatData( int64 revision,bool urgent /* = false */ )
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

	if (!urgent && revision < g_pInst->GetSaveUpdateTime() + GameSrvCfg::Instance().PlatCacheSaveUpdateTime())
	{
		return false;
	}

	map<string, int64>::iterator itRes;
	map<string,DB_Plat*> &plats = g_pInst->GetPlatList();
//	map<string,int64> &mapRevision = g_pInst->GetPlatRevision();
	map<string,DB_Plat*>::iterator iter = plats.begin();

	int save_interval = GameSrvCfg::Instance().PlatCacheSaveInterval();		//10s
	while(iter!=plats.end())
	{
		map<string,DB_Plat*>::iterator oiter = iter;
		iter ++;
		DB_Plat *pDBPlat = oiter->second;
		if (pDBPlat == NULL||!pDBPlat->has_platform_id())
		{
			continue;
		}
		string platID = pDBPlat->platform_id();
		int64 nRevision = pDBPlat->ltmemrevision();
		//int64 nRevision = 0;

		//itRes = mapRevision.find(platID);
		//if(itRes !=mapRevision.end())
		//{
		//	nRevision = itRes->second;
		//}
		if( urgent || nRevision < revision - save_interval)
		{
			string plat_info;
			pDBPlat->SerializeToString(&plat_info);
			plat_info = "new:" + plat_info;
			bool sucess = MemCacheServerHandler::UpdatePlatInfo(platID,plat_info);
			if (sucess)
			{
				plats.erase(oiter);
				//if(itRes != mapRevision.end())
				//{
				//	mapRevision.erase(itRes);
				//}
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllPlatData"<<platID);
				//LOG4CXX_ERROR(logger_,"MemCacheServerHandler::sucess");
			}
		}
	}

	g_pInst->SetSaveUpdateTime(revision);
	return true;
}



