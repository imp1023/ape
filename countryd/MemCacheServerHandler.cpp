
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
#include "../common/Clock.h"
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

	SaveAllCountryMemDBData(revision,urgent);
	g_pInst->SetSaveUpdateTime(revision);
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