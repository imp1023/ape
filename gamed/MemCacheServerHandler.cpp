/* 
* File:   MemCacheServerHandler.cpp
* Author: Kidd
* 
* Created on 2011年12月23日, 上午10:32
*/

#include <list>
#include "MemCacheServerHandler.h"
#include "../event/EventQueue.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/counter.h"
#include "../event/EventDefine.h"
#include "../gamed/GameEventHandler.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"

#include "../logic/User.h"
#include "../gamed/GameDataHandler.h"
#include "../common/Clock.h"
#include "../common/Ini.h"

MemCacheServerHandler::MSH_T* MemCacheServerHandler::g_pInst = NULL;
log4cxx::LoggerPtr MemCacheServerHandler::logger = log4cxx::Logger::getLogger("MemCacheServer");

MemCacheServerHandler::MemCacheServerHandler()
{
	m_bEnabled			= false;
	m_bInited			= false; 
	m_eh				= NULL;
	m_RemoveUpdate		= Clock::getCurrentSystemTime();
	m_SaveUpdate		= Clock::getCurrentSystemTime();
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

int MemCacheServerHandler::GetServerId(const int64 &uid)
{
	int hash = getUidHash(uid);
	return hash % GetServerNumber() + 1;
}

bool MemCacheServerHandler::GetUserInfo(const int64 &uid, std::string& dist)//,std::string& npcstring,std::string& effstring)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

// 	if (uid != 2928562105553271)
// 	{
// 		return false;
// 	}

	int     dbid    = g_pInst->GetServerId(uid);
	TCRDB*  pConn   = g_pInst->GetDB(dbid);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_get_connect_null_error");
		return false;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);

	int klen = 16;
	int len  = 0;
	//char* buffer    = (char*) tcrdbget2(pConn, key.c_str());
	char* buffer = (char*) tcrdbget(pConn, suid, klen, &len);
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

// 	std::string key = "Eff_" + toString(uid) ;
// 	klen = key.length();
// 	len  = 0;
// 	char* buffereff = (char*)tcrdbget(pConn, key.c_str(), klen, &len);
// 	if(buffereff == NULL)
// 	{
// 		int ecode = tcrdbecode(pConn);
// 		if (ecode != TTENOREC)
// 		{
// 			g_pInst->SetEnbale(false);
// 			LOG4CXX_ERROR(logger,"memcache_get_ecode_error");
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		std::string outeffbuf(buffereff, len);
// 		effstring = outeffbuf;
// 		free(buffereff);
// 	}
// 	
// 
// 	key = "NPC_" + toString(uid) ;
// 	klen = key.length();
// 	len  = 0;
// 
// 	char* buffernpc = (char*)tcrdbget(pConn, key.c_str(), klen, &len);
// 	if(buffernpc == NULL)
// 	{
// 		int ecode = tcrdbecode(pConn);
// 		if (ecode != TTENOREC)
// 		{
// 			g_pInst->SetEnbale(false);
// 			LOG4CXX_ERROR(logger,"memcache_get_ecode_error");
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		std::string outnpcbuf(buffernpc, len);
// 		npcstring = outnpcbuf;
// 		free(buffernpc);
// 	}
		
	//g_pInst->GetCounter().increase("mem_get_user");
	return true;
}

bool MemCacheServerHandler::UpdateUserInfo(const int64 &uid,std::string& dist)//,std::string& npcstring,std::string& effstring)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
	time_t time_first = Clock::getUTime();
	int     dbid    = g_pInst->GetServerId(uid);
	TCRDB*  pConn   = g_pInst->GetDB(dbid);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_put_connect_null_error");
		return false;
	}

	const char* buf = dist.c_str();
	int         len = dist.length();
	char suid[32];
	sprintf(suid, "%016llX", uid);
	int klen = 16;
	if (!tcrdbput(pConn, suid, klen, buf, len))
	{
		int ecode = tcrdbecode(pConn);
		g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_update_put_error"<<tcrdberrmsg(ecode) << uid);
		return false;
	}
	
// 	buf = effstring.c_str();
// 	len = effstring.length();
// 	
// 	std::string key = "Eff_" + toString(uid) ;
// 	klen = key.length();
// 
// 	if (!tcrdbput(pConn, key.c_str(), klen, buf, len))
// 	{
// 		int ecode = tcrdbecode(pConn);
// 		g_pInst->SetEnbale(false);
// 		LOG4CXX_ERROR(logger,"memcache_update_put_error"<<tcrdberrmsg(ecode) << uid);
// 		return false;
// 	}
// 
// 	buf = npcstring.c_str();
// 	len = npcstring.length();
// 
// 	key = "NPC_" + toString(uid) ;
// 	klen = key.length();
// 
// 	if (!tcrdbput(pConn, key.c_str(), klen, buf, len))
// 	{
// 		int ecode = tcrdbecode(pConn);
// 		g_pInst->SetEnbale(false);
// 		LOG4CXX_ERROR(logger,"memcache_update_put_error"<<tcrdberrmsg(ecode) << uid);
// 		return false;
// 	}
	
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
		if (myini.Open("MemErr.ini") == false)
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

void MemCacheServerHandler::SetEventHandler( GameEventHandler* eh )
{
	m_eh = eh;
}

bool MemCacheServerHandler::RemoveUserInfo( const int64 &uid )
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
	int     dbid    = g_pInst->GetServerId(uid);
	TCRDB*  pConn   = g_pInst->GetDB(dbid);
	if (pConn == NULL)
	{
		g_pInst->SetEnbale(false);
		int ecode = tcrdbecode(pConn);
		LOG4CXX_ERROR(logger,"memcache_remove_connect_null_error"<<tcrdberrmsg(ecode));
		return false;
	}
	char suid[32];
	sprintf(suid, "%016llX", uid);
	int klen = 16;
	if (!tcrdbout(pConn, suid, klen))
	{
		int ecode = tcrdbecode(pConn);
		//g_pInst->SetEnbale(false);
		LOG4CXX_ERROR(logger,"memcache_remove_out_error"<<tcrdberrmsg(ecode)<<uid);
		return false;
	}


	std::string key = "Eff_" + toString(uid) ;
	klen = key.length();

	if (!tcrdbout(pConn, key.c_str(), klen))
	{
		int ecode = tcrdbecode(pConn);
		LOG4CXX_ERROR(logger,"memcache_remove_out_error"<<tcrdberrmsg(ecode) << uid);
		return false;
	}

	key = "NPC_" + toString(uid) ;
	klen = key.length();

	if (!tcrdbout(pConn, key.c_str(), klen))
	{
		int ecode = tcrdbecode(pConn);
		LOG4CXX_ERROR(logger,"memcache_remove_out_error"<<tcrdberrmsg(ecode) << uid);
		return false;
	}

	//g_pInst->GetCounter().increase("mem_remove_cnt");
	return true;
}

bool MemCacheServerHandler::SafePushUser(const int64 &uid,User * pUser)
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
//	g_pInst->acquireLock();
// 	if (!pUser->Online())
// 	{
// 		return false;
// 	}
	
	map<int64,User*> & user_map = g_pInst->GetUserList();
	user_map[uid] = pUser;
//	g_pInst->releaseLock();
	return true;
}

bool MemCacheServerHandler::UpdateRemoveUser()
{
	return false;
//
//	if (g_pInst == NULL || !g_pInst->CanUse())
//	{
//		return false;
//	}
//	
//	time_t timeNow = Clock::getCurrentSystemTime();
//	time_t last_time = g_pInst->GetRemoveUpdateTime();
//	if (timeNow - last_time < GameSrvCfg::Instance().CacheRemoveUpdateTime())
//	{
//		return false;
//	}
//
////	vector<int64> vecRemoveUser;
////	g_pInst->acquireLock();
//	map<int64,bool>& remove_list = g_pInst->GetRemoveList();
////	map<int64,bool> remove_temp_list = remove_list;
////	g_pInst->releaseLock();
//
//	/*Counter & counter = g_pInst->GetCounter();
//	Statistics& stat = g_pInst->GetStatistics();*/
//
//	GameEventHandler * eh = g_pInst->GetEventHandler();
//	const hash_map<int64, User*> & user_map = eh->getDataHandler()->getUsers();
//	const map<int64, User*> &dirty_map = eh->getDataHandler()->getDirtyUsers();
//	int free_cnt(0);
//
//	map<int64,bool>::iterator iter = remove_list.begin();
//	while(iter != remove_list.end())
//	{
//		map<int64, bool>::iterator oiter = iter;
//		iter ++;
//		int64 uid = oiter->first;
//		bool remove_flag(false);
//		do 
//		{
//			hash_map<int64, User*>::const_iterator iter_user = user_map.find(uid);
//			if (iter_user == user_map.end() )
//			{
//				remove_flag = true;
//				break;
//			}
//
//			User * pUser = iter_user->second;
//			if (pUser == NULL)
//			{
//				remove_flag = true;
//				break;
//			}
//
//			if (pUser->Online())
//			{
//				break;
//			}
//
//			map<int64, User*>::const_iterator iter_dirty = dirty_map.find(pUser->GetUid());
//			if (iter_dirty != dirty_map.end())
//			{
//				break;
//			}
//
//			if (timeNow < pUser->getMemRevision() + GameSrvCfg::Instance().CacheRemoveTime() *1000/*3600 * 1000 * 24*/)
//			{//delete less than 24h
//				break;
//			}
//
//			remove_flag = true;
//
//		} while (0);
//
//
//		if (remove_flag)
//		{
//			g_pInst->RemoveUserInfo(uid);
//			remove_list.erase(oiter);
//			free_cnt++;
//		}
//
//		if (free_cnt >= GameSrvCfg::Instance().CacheFreeCnt())
//		{
//			break;
//		}
//
//		time_t time_last = Clock::getCurrentSystemTime();
//		if (time_last - timeNow > 20)
//		{//more than 20ms return
//			break;
//		}
//	}
//
//	g_pInst->SetRemoveUpdateTime(timeNow);
//	return true;
}

//Statistics & MemCacheServerHandler::GetStatistics()
//{
//	Statistics temp;
//	if (g_pInst == NULL || !g_pInst->CanUse())
//	{
//		return temp;
//	}
//	return g_pInst->GetEventHandler()->getDataHandler()->getStatistics();
//}
//
//Counter & MemCacheServerHandler::GetCounter()
//{
//	Counter temp;
//	if (g_pInst == NULL || !g_pInst->CanUse())
//	{
//		return temp;
//	}
//	return g_pInst->GetEventHandler()->getDataHandler()->getCounter();
//}

bool MemCacheServerHandler::SafePushRemoveList( const int64& uid )
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}
	map<int64,bool> &remove_list = g_pInst->GetRemoveList();
	remove_list[uid] = true;
	return true;
}

bool MemCacheServerHandler::SaveAllUserData( int64 revision,bool urgent /* = false */ )
{
	if (g_pInst == NULL || !g_pInst->CanUse())
	{
		return false;
	}

	if (!urgent && revision < g_pInst->GetSaveUpdateTime() + GameSrvCfg::Instance().CacheSaveUpdateTime())
	{
		return false;
	}
	time_t tmStart = Clock::getCurrentSystemTime();
	
	map<int64, User*> &users = g_pInst->GetUserList();
	map<int64, User*>::iterator iter = users.begin();
	int save_interval = GameSrvCfg::Instance().CacheSaveInterval();		//10s
	int nSize = users.size();
	while(iter!=users.end())
	{
		map<int64, User*>::iterator oiter = iter;
		iter ++;
		User *user = oiter->second;
		if (user == NULL)
		{
			continue;
		}
		if( urgent || user->getMemRevision() < revision - save_interval)
		{
			time_t userTMStart = Clock::getCurrentSystemTime();

			string user_info;
			//user->GetDbUser().set_version(user->GetDbUser().version()+1);
			user->GetDbUser().SerializeToString(&user_info);
// 			string effect_info;
// 			user->GetDBEffect().SerializeToString(&effect_info);
// 			string npclist_info;
// 			user->GetDBNPCList().SerializeToString(&npclist_info);
			bool sucess = MemCacheServerHandler::UpdateUserInfo(user->GetUid(),user_info);//,npclist_info,effect_info);
			if (sucess)
			{
				g_pInst->SafePushRemoveList(user->GetUid());
				users.erase(oiter);
				LOG4CXX_DEBUG(logger,"MemCacheServerHandler::SaveAllUserData"<<user->GetUid());
				//LOG4CXX_ERROR(logger_,"MemCacheServerHandler::sucess");
			}
			//time_t userTMEnd = Clock::getCurrentSystemTime();
			//userTMEnd -= userTMStart;
			//if(userTMEnd>30)
			//{
			//	SYS_LOG(user->GetUid(),LT_SrvDebug,0,0,"MemSaveOne"<<(int)userTMEnd<<(int)user_info.size()<<(int)effect_info.size()<<(int)npclist_info.size());
			//}
		}
	}
	
	g_pInst->SetSaveUpdateTime(revision);

	//time_t tmEnd = Clock::getCurrentSystemTime();
	//tmEnd -= tmStart;
	//if(tmEnd > 100)
	//{//超过500毫秒
	//	SYS_LOG(0,LT_SrvDebug,0,0,"MemSaveAll"<<(int)tmEnd<<nSize);
	//}
	return true;
}



