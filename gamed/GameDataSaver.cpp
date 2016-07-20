#define _CRT_SECURE_NO_WARNINGS
#include "../logic/dbinterface.pb.h"
#include "GameDataSaver.h"
#include <iomanip>
#include "GameDataHandler.h"
#include <sys/types.h>
#include "../common/Clock.h"
#include "../common/HotCacheDBCfg.h"
#include "../common/distribution.h"
#include "../common/Msg2QQ.h"
#include "../gamed/MemCacheServerHandler.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

extern ServerConfig serverConfig;

GameDataSaver::GameDataSaver(int nid)
{
	nid_ = nid;
	db_num = serverConfig.dbNum();
	sprintf(allocate_file, "allocate_g%d.cfg", nid_);
	sprintf(allocate_dbstr,"ALLOCATE_G%05d",nid_);
	logger_ = log4cxx::Logger::getLogger("GameDataSaver");
	alloc_rdb = NULL;
	alloc_rdb2= NULL;
	//plat_tmp_rdb = NULL;
	//user_tmp_rdb = NULL;
	log_time = 0;
	count_dirty_ = 0;

	initDatabase();

	pthread_mutex_init(&load_user_mutex_, NULL);
	pthread_mutex_init(&save_user_mutex_, NULL);
	pthread_mutex_init(&load_map_mutex_, NULL);
	pthread_mutex_init(&save_map_mutex_, NULL);

	b_serving = true;

	m_bHotCacheEnable		= HotCacheDBCfg::Instance().EnableWrite();
	//如果不可写，则不可读
	m_bHotCacheEnableLoad	= HotCacheDBCfg::Instance().EnableRead();
	m_bHotCacheEnableLoadNPC= HotCacheDBCfg::Instance().EnableReadNPC();
	initThreads();
}

GameDataSaver::~GameDataSaver()
{
	termThreads();

	pthread_mutex_destroy(&load_user_mutex_);
	pthread_mutex_destroy(&save_user_mutex_);
	pthread_mutex_destroy(&load_map_mutex_);
	pthread_mutex_destroy(&save_map_mutex_);
}

void GameDataSaver::initThreads()
{
	int ret = 0;
	ret = pthread_create(&th_user_load, NULL, GameDataSaver::loadUserThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating load data thread");
	}
	ret = pthread_create(&th_user_save, NULL, GameDataSaver::saveUserThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating save data thread");
	}

	//ret = pthread_create(&th_map_load, NULL, GameDataSaver::loadMapThread, (void *)this);
	//if(ret != 0)
	//{
	//	LOG4CXX_ERROR(logger_, "ERROR creating load map thread");
	//}

	//ret = pthread_create(&th_map_save, NULL, GameDataSaver::saveMapThread, (void *)this);
	//if(ret != 0)
	//{
	//	LOG4CXX_ERROR(logger_, "ERROR creating save thread");
	//}
}

void GameDataSaver::termThreads()
{
	b_serving = false;
	pthread_join(th_user_load, NULL);
	//pthread_join(th_map_load, NULL);
	//pthread_join(th_map_save, NULL);
	pthread_join(th_user_save, NULL);

	LOG4CXX_ERROR(logger_, "Threads terminated. Quit.");
	termDatabase();
}

void GameDataSaver::initDatabase()
{
	void * tmp = NULL;
	for(int i = 0; i < db_num; i ++)
	{
		db_user_load.insert(make_pair(i + 1, tmp));
		db_user_save.insert(make_pair(i + 1, tmp));

		db_user_load2.insert(make_pair(i + 1, tmp));
		db_user_save2.insert(make_pair(i + 1, tmp));

		hotcache_user_load.insert(make_pair(i + 1, tmp));
		hotcache_user_save.insert(make_pair(i + 1, tmp));

	}
}

void GameDataSaver::termDatabase()
{
	for(int i = 0; i < db_num; i ++)
	{
		TCRDB* rdb = (TCRDB*)db_user_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_user_load[i] = NULL;
		}

		rdb = (TCRDB*)db_user_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_user_save[i] = NULL;
		}


		//备
		rdb = (TCRDB*)db_user_load2[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_user_load2[i] = NULL;
		}

		rdb = (TCRDB*)db_user_save2[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_user_save2[i] = NULL;
		}


		//内存TT
		rdb = (TCRDB*)hotcache_user_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_user_load[i] = NULL;
		}

		rdb = (TCRDB*)hotcache_user_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_user_save[i] = NULL;
		}


	}
	db_user_load2.clear();
	db_user_save2.clear();


	db_user_load.clear();
	db_user_save.clear();

	hotcache_user_load.clear();
	hotcache_user_save.clear();

	if(alloc_rdb != NULL)
	{
		tcrdbclose(alloc_rdb);
		tcrdbdel(alloc_rdb);
		alloc_rdb = NULL;
	}
	if(alloc_rdb2 != NULL)
	{
		tcrdbclose(alloc_rdb2);
		tcrdbdel(alloc_rdb2);
		alloc_rdb2 = NULL;
	}
}

//void GameDataSaver::loadAllocateSetting(int64& uid, int64& iid)
//{
//	if(alloc_rdb == NULL)
//	{
//		alloc_rdb = getAllocateDb(1);
//	}
//	char* buffer = NULL;
//	if(alloc_rdb != NULL)
//	{
//		buffer = (char*)tcrdbget2(alloc_rdb, allocate_dbstr);
//	}
//	if(buffer != NULL)
//	{
//		sscanf(buffer, "%lld,%lld", &uid, &iid);
//		free(buffer);
//	}
//	else
//	{
//		//if not in db, read info from file,this is use for change info from file to db,will delete later.
//		FILE *fp = fopen(allocate_file, "r");
//		if(NULL != fp)
//		{
//			char setting[256];
//			fgets(setting, 256, fp);
//			if(strlen(setting) >= 7)
//			{
//				sscanf(setting, "%lld,%lld", &uid, &iid);
//			}
//			fclose(fp);
//		}
//	}
//}
//
//void GameDataSaver::saveAllocateSetting(int64& uid, int64& iid)
//{
//	if(alloc_rdb == NULL)
//	{
//		alloc_rdb = getAllocateDb(1);
//	}
//	if(alloc_rdb != NULL)
//	{
//		char setting[256];
//		sprintf(setting, "%lld,%lld", uid, iid);
//		if(!tcrdbput2(alloc_rdb, allocate_dbstr, setting))
//		{
//			LOG4CXX_ERROR(logger_, "Put allocate ERROR : " << tcrdberrmsg(tcrdbecode(alloc_rdb)));
//			tcrdbclose(alloc_rdb);
//			tcrdbdel(alloc_rdb);
//			alloc_rdb = NULL;
//		}
//	}
//
//	if(alloc_rdb2 == NULL)
//	{
//		alloc_rdb2 = getAllocateDb(2);
//	}
//	if(alloc_rdb2 != NULL)
//	{
//		char setting[256];
//		sprintf(setting, "%lld,%lld", uid, iid);
//		if(!tcrdbput2(alloc_rdb2, allocate_dbstr, setting))
//		{
//			LOG4CXX_ERROR(logger_, "Put allocate ERROR : " << tcrdberrmsg(tcrdbecode(alloc_rdb2)));
//			tcrdbclose(alloc_rdb2);
//			tcrdbdel(alloc_rdb2);
//			alloc_rdb2 = NULL;
//		}
//	}
//
//	FILE *fp = fopen(allocate_file, "w");
//	if(NULL != fp)
//	{
//		fprintf(fp, "%lld,%lld", uid, iid);
//		fclose(fp);
//	}
//}


void GameDataSaver::writeSaveListLength(int len)
{
	time_t now = time(NULL);
	if(now != log_time && now % 10 == 0)
	{
		log_time = now;
		char logfile[256];
		sprintf(logfile,"savelist%d.log",nid_);
		FILE *fp = fopen(logfile, "a");
		if(NULL != fp)
		{
			struct tm *tmnow = NULL;
			char szTime[128];
			tmnow = localtime(&now);
			strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", tmnow);
			fprintf(fp, "%s - list cnt:%d , dirty cnt: %d \n", szTime, len, count_dirty_);
			fclose(fp);
		}
	}
}


void GameDataSaver::writeSaveError(const char *strError,const char* szKey,int nValueLen)
{
	time_t now = time(NULL);
	char logfile[256];
	sprintf(logfile,"saveerror%d.log",nid_);
	FILE *fp = fopen(logfile, "a");
	if(NULL != fp)
	{
		struct tm *tmnow = NULL;
		char szTime[128];
		tmnow = localtime(&now);
		strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", tmnow);
		fprintf(fp, "%s -save db error,key:%s,value len:%d,error:%s\n", szTime,szKey,nValueLen, strError);
		fclose(fp);
	}
}

int GameDataSaver::safeSaveAllUser(time_t revision, GameDataHandler* const dh, bool force)
{
	// user data
	pthread_mutex_lock(&save_user_mutex_);
	hash_map<int64, User*> &users = dh->dirty_users_;
	hash_map<int64, User*>::iterator iter = users.begin();
	int list_len = save_user_list_.size();
	int save_interval = 1;
	if(list_len > 250)
	{
		save_interval += (list_len - 250) / 50;
		if(save_interval > 20)
		{
			save_interval = 20;
		}
	}
	while(iter!=users.end())
	{
		hash_map<int64, User*>::iterator oiter = iter;
		++iter;
		User *user = oiter->second;
		if(force || user->revision() < revision - save_interval * 600 * 1000)
		{
			pushSaveUser(dh, user->GetUid(), user);
			users.erase(oiter);
		}
	}
	count_dirty_ = users.size();
	pthread_mutex_unlock(&save_user_mutex_);
	return 0;
}

int GameDataSaver::safeSaveUser(GameDataHandler* const dh, User* user)
{
	if(user != NULL)
	{
		pthread_mutex_lock(&save_user_mutex_);
		pushSaveUser(dh, user->GetUid(), user);
		pthread_mutex_unlock(&save_user_mutex_);
		LOG4CXX_DEBUG(logger_, "Save user data:uid=" << user->GetUid());
	}
	return 0;
}

int GameDataSaver::safeLoadUser(GameDataHandler* const dh, int64& uid)
{
	pthread_mutex_lock(&load_user_mutex_);
	pushLoadUser(dh, uid);
	pthread_mutex_unlock(&load_user_mutex_);
	LOG4CXX_DEBUG(logger_, "Load user data " << uid);
	return 0;
}

int GameDataSaver::pushSaveUser(GameDataHandler* const dh, int64 uid, User* user)
{
	string * data = new string;
	//string * effdata = new string;
	string effdata;
	string * npcdata = new string;
	MakeUserString(dh, user, *data);//,effdata,*npcdata);
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		if (MemCacheServerHandler::UpdateUserInfo(uid, *data/*,*npcdata,effdata*/) == true)
		{
			MemCacheServerHandler::SafePushRemoveList(uid);
		}
	}
	list<UserItem>::iterator iter = save_user_list_.begin();
	while(iter != save_user_list_.end())
	{
		if(iter->uid == uid)
		{
			delete iter->data;
			//delete iter->effdata;
			delete iter->npcdata;
			iter->data = data;
			//iter->effdata = effdata;
			iter->npcdata = npcdata;
			return 0;
		}
		++iter;
	}

	UserItem save_data;
	save_data.dh = dh;
	save_data.data = data;
	//save_data.effdata = effdata;
	save_data.npcdata = npcdata;
	save_data.uid = uid;

	save_user_list_.push_front(save_data);
	//user->setRevision(dh->revision());
	return 0;
}

int GameDataSaver::pushLoadUser(GameDataHandler* const dh, int64& uid)
{
	UserItem load_data;
	load_data.dh = dh;
	load_data.data = NULL;
	load_data.uid = uid;
//	load_data.effdata = NULL;
	load_data.npcdata = NULL;

	load_user_list_.push_front(load_data);
	return 0;
}

TCRDB* GameDataSaver::getDb(const string& addr, int port)
{
	TCRDB* rdb = tcrdbnew();
	if(!tcrdbopen(rdb, addr.c_str(), port))
	{
		LOG4CXX_ERROR(logger_, "open db[" << addr << ":" << port
			<<"] error: " << tcrdberrmsg(tcrdbecode(rdb)));
		tcrdbdel(rdb);
		rdb = NULL;
	}

	return rdb;
}

TCRDB* GameDataSaver::getUserLoadDb(int dbid,int type)
{
	if(type==1)
	{
		TCRDB *rdb = (TCRDB *)db_user_load[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbUserAddr1(dbid), serverConfig.dbUserPort1(dbid));
		}
		if(rdb != NULL)
		{
			db_user_load[dbid] = rdb;
		}
		return rdb;
	}
	else if(type==2)
	{
		TCRDB *rdb = (TCRDB *)db_user_load2[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbUserAddr2(dbid), serverConfig.dbUserPort2(dbid));
		}
		if(rdb != NULL)
		{
			db_user_load2[dbid] = rdb;
		}
		return rdb;
	}
	return NULL;

}

TCRDB* GameDataSaver::getUserSaveDb(int dbid,int type)
{
	if(type==1)
	{
		TCRDB *rdb = (TCRDB *)db_user_save[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbUserAddr1(dbid), serverConfig.dbUserPort1(dbid));
		}
		if(rdb != NULL)
		{
			db_user_save[dbid] = rdb;
		}
		return rdb;
	}
	else if(type==2)
	{
		TCRDB *rdb = (TCRDB *)db_user_save2[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbUserAddr2(dbid), serverConfig.dbUserPort2(dbid));
		}
		if(rdb != NULL)
		{
			db_user_save2[dbid] = rdb;
		}
		return rdb;
	}
	return NULL;

}

//}
//
//int GameDataSaver::saveUserNPC(GameDataHandler* const dh, int64& uid, string& data,int type)
//{
//	time_t ltStart = Clock::getCurrentSystemTime();
//	int dbid = getUidHash(uid) % db_num + 1;
//	TCRDB* rdb = getUserSaveDb(dbid,type);
//	if(rdb == NULL)
//	{
//		return -1;
//	}
//	std::string key = "NPC_" + toString(uid) ;
//	int klen = key.length();
//	const char* buf = data.c_str();
//	int len = data.length();
//	if(!tcrdbput(rdb, key.c_str(), klen, buf, len))
//	{
//		int ecode = tcrdbecode(rdb);
//		LOG4CXX_ERROR(logger_, "Put user NPC ERROR : uid:" <<uid <<",valuelength:"<< len << ",type:"<<type<<",error:"<< tcrdberrmsg(ecode));
//
//		writeSaveError(tcrdberrmsg(ecode),key.c_str(),len);
//		tcrdbclose(rdb);
//		tcrdbdel(rdb);
//		if(type == 1)
//		{
//			db_user_save[dbid] = NULL;
//		}
//		else if(type == 2)
//		{
//			db_user_save2[dbid] = NULL;
//		}
//
//		return -1;
//	}
//	LOG4CXX_DEBUG(logger_, "Saved UserNPC: uid="<<uid);
//	return 0;
//}
//int GameDataSaver::saveUserEffect(GameDataHandler* const dh, int64& uid, string& data,int type)
//{
//
//	int dbid = getUidHash(uid) % db_num + 1;
//	TCRDB* rdb = getUserSaveDb(dbid,type);
//	if(rdb == NULL)
//	{
//		return -1;
//	}
//
//	std::string key = "Eff_" + toString(uid);
//	int klen = key.length();
//	const char* buf = data.c_str();
//	int len = data.length();
//	if(!tcrdbput(rdb, key.c_str(), klen, buf, len))
//	{
//		int ecode = tcrdbecode(rdb);
//		LOG4CXX_ERROR(logger_, "Put user Effect ERROR : uid:" <<uid <<",valuelength:"<< len << ",type:"<<type<<",error:"<< tcrdberrmsg(ecode));
//		writeSaveError(tcrdberrmsg(ecode),key.c_str(),len);
//		tcrdbclose(rdb);
//		tcrdbdel(rdb);
//		if(type == 1)
//		{
//			db_user_save[dbid] = NULL;
//		}
//		else if(type == 2)
//		{
//			db_user_save2[dbid] = NULL;
//		}
//
//		return -1;
//	}
//	
//
//	LOG4CXX_DEBUG(logger_, "Saved UserEffect: uid="<<uid);
//	return 0;
//}

int GameDataSaver::saveUser(GameDataHandler* const dh, int64& uid, string& data,int type)
{
	//time_t ltStart = Clock::getCurrentSystemTime();
	int dbid = getUidHash(uid) % db_num + 1;
	TCRDB* rdb = getUserSaveDb(dbid,type);
	if(rdb == NULL)
	{
		return -1;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, suid, 16, buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put user ERROR : uid:" <<uid <<",valuelength:"<< len << ",type:"<<type<<",error:"<< tcrdberrmsg(ecode));

		writeSaveError(tcrdberrmsg(ecode),suid,len);
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		if(type == 1)
		{
			db_user_save[dbid] = NULL;
		}
		else if(type == 2)
		{
			db_user_save2[dbid] = NULL;
		}
		
		return -1;
	}
	if(type==1)
	{
		DelUserEffectInfo(rdb,uid);
		DelUserNPCInfo(rdb,uid);
	}
	//time_t ltEnd = Clock::getCurrentSystemTime();

	//CMsg2QQDB::GetInstance()->TellSaveDB(int(ltEnd-ltStart),
	//	serverConfig.gamedIp(nid_).c_str(),
	//	serverConfig.dbUserAddr1(dbid).c_str(),
	//	serverConfig.dbUserPort1(dbid));
	LOG4CXX_DEBUG(logger_, "Saved User: uid="<<uid);
	return 0;
}
void GameDataSaver::DelUserNPCInfo(TCRDB* pDB,int64& uid)
{
	if(pDB==NULL)
		return;
	std::string key = "NPC_" + toString(uid);
	tcrdbout2(pDB,key.c_str());
}

void GameDataSaver::DelUserEffectInfo(TCRDB* pDB,int64& uid)
{
	if(pDB==NULL)
		return;
	std::string key = "Eff_" + toString(uid);
	tcrdbout2(pDB,key.c_str());
}

//bool GameDataSaver::_TmpGetUser(TCRDB* pDB,int64 uid,DB_User& dbUser)
//{
//	char suid[32];
//	sprintf(suid, "%016llX", uid);
//	int len;
//	char* buffer = (char*)tcrdbget(pDB, suid, 16, &len);
//	if(buffer == NULL)
//	{
//		int ecode = tcrdbecode(pDB);
//		if(ecode == TTENOREC) {
//			return false;
//		}
//		else
//		{
//			LOG4CXX_ERROR(logger_, "Get user ERROR : " << tcrdberrmsg(ecode));
//			exit(0);
//		}
//	}
//	else
//	{
//		LOG4CXX_DEBUG(logger_, "Loaded User: uid="<<uid);
//		string data(buffer, len);
//		dbUser.ParseFromString( data );
//		free(buffer);
//		return true;
//	}
//	return false;
//}
//

//int GameDataSaver::_TmploadUser(GameDataHandler* const dh, int64& uid)
//{
//	int dbid = getUidHash(uid) % db_num + 1;
//	if(dbid != 20)
//	{
//		LOG4CXX_ERROR(logger_," GameDataSaver::_TmploadUser ERROR!!!!!!!!!!!");
//		exit(0);
//	}
//	if(user_tmp_rdb == NULL)
//	{
//		user_tmp_rdb = getDb(serverConfig.dbUserAddr2(dbid), serverConfig.dbUserPort2(dbid));
//	}
//	if(user_tmp_rdb == NULL)
//	{
//		LOG4CXX_ERROR(logger_," GameDataSaver::_TmploadUser GetDB ERROR!!!!!!!!!!!");
//		exit(0);
//	}
//	DB_User dbUser1;
//	//先读备份库
//	bool bHasUser1 = _TmpGetUser(user_tmp_rdb,uid,dbUser1);
//
//	TCRDB* rdb = getUserLoadDb(dbid);
//	if(rdb == NULL)
//	{
//		LOG4CXX_ERROR(logger_," GameDataSaver::_TmploadUser GetDB ERROR!!!!!!!!!!!");
//		exit(0);
//	}
//	DB_User dbUser2;
//	bool    bHasUser2 = _TmpGetUser(rdb,uid,dbUser2);
//
//	User* user = NULL;
//	DB_User* pSelDBUser = NULL;
//	if(!bHasUser1 && !bHasUser2)
//	{//新用户
//		dh->updateLoadUser(uid, user);
//		return 1;
//	}
//	else if(bHasUser1 && bHasUser2)
//	{//两个库都有
//		if(dbUser1.mutable_player()->level() > dbUser2.mutable_player()->level())
//		{
//			pSelDBUser = &dbUser1;
//		}
//		else
//		{
//			pSelDBUser = &dbUser2;
//		}
//	}
//	else if(bHasUser1)
//	{
//		pSelDBUser = &dbUser1;
//	}
//	else
//	{
//		pSelDBUser = &dbUser2;
//	}
//	if(pSelDBUser == NULL)
//	{
//		LOG4CXX_ERROR(logger_, "GameDataSaver::_TmploadUser(GameDataHandler* const dh, int64& uid) ERROR!!! " );
//		exit(0);
//	}
//	else
//	{
//		user = new User();
//		dh->acquireDataLock();
//		user->SetDbUser(*pSelDBUser);
//		dh->releaseDataLock();
//		dh->updateLoadUser(uid, user);
//	}
//	return 0;
//}	
//

//int GameDataSaver::loadUserEffect(GameDataHandler* const dh,DB_Effect* pEffect, int64& uid,int type)
//{
//	if (pEffect==NULL)
//		return -1;
//	int dbid = getUidHash(uid) % db_num + 1;
//
//	TCRDB* rdb = getUserLoadDb(dbid,type);
//	if(rdb == NULL)
//	{
//		return -1;
//	}
//
//	std::string key = "Eff_" + toString(uid) ;
//	int klen = key.length();
//	int len  = 0;
//
//	char* buffer = (char*)tcrdbget(rdb, key.c_str(), klen, &len);
//	if(buffer == NULL)
//	{
//		int ecode = tcrdbecode(rdb);
//		if(ecode == TTENOREC) {
//			LOG4CXX_DEBUG(logger_, "Loaded User Effect is Empty: uid="<<uid);
//			return 1;
//		}
//		LOG4CXX_ERROR(logger_, "Get user Effect ERROR : " << tcrdberrmsg(ecode));
//		return -1;
//	}
//	LOG4CXX_DEBUG(logger_, "Loaded User NPC: uid="<<uid);
//
//	string data(buffer, len);
//	
//	dh->acquireDataLock();
//	LoadUserEffectFromString(pEffect, data);
//	dh->releaseDataLock();
//	free(buffer);
//	return 0;
//}

/*
int GameDataSaver::loadUserNPC(GameDataHandler* const dh,DB_NPCList* pNpcList, int64& uid)
{
	if (pNpcList == NULL)
		return -1;
	int dbid = getUidHash(uid) % db_num + 1;
	int len  = 0;
	char* buffer = (char*) loadUserNPCFromHotCache(dh,uid,len);
	if(buffer==NULL)
	{
		LOG4CXX_DEBUG(logger_, "Loaded User NPC is Empty: uid="<<uid);
		return 1;

		//TCRDB* rdb = getUserLoadDb(dbid,type);
		//if(rdb == NULL)
		//{
		//	return -1;
		//}
		//std::string key = "NPC_" + toString(uid) ;
		//int klen = key.length();
		//

		//buffer = (char*)tcrdbget(rdb, key.c_str(), klen, &len);
		//if(buffer == NULL)
		//{
		//	int ecode = tcrdbecode(rdb);
		//	if(ecode == TTENOREC) {
		//		LOG4CXX_DEBUG(logger_, "Loaded User NPC is Empty: uid="<<uid);
		//		return 1;
		//	}
		//	LOG4CXX_ERROR(logger_, "Get user NPC ERROR : " << tcrdberrmsg(ecode));
		//	return -1;
		//}
		//LOG4CXX_DEBUG(logger_, "Loaded User NPC: uid="<<uid);
	}

	string data(buffer, len);
	dh->acquireDataLock();
	LoadUserNpcFromString(pNpcList, data);
	dh->releaseDataLock();
	free(buffer);
	return 0;
}
*/
int GameDataSaver::loadUser(GameDataHandler* const dh, int64& uid,int type)
{
	int dbid = getUidHash(uid) % db_num + 1;

	char suid[32];
	sprintf(suid, "%016llX", uid);

	User* user = NULL;
	int len = 0;
	//time_t ltStart = Clock::getCurrentSystemTime();
	char* buffer = (char*) loadUserFromHotCache(dh,uid,len);
	if(buffer==NULL)
	{
		TCRDB* rdb = getUserLoadDb(dbid,type);
		if(rdb == NULL)
		{
			return -1;
		}
		buffer = (char*)tcrdbget(rdb, suid, 16, &len);
		if(buffer == NULL)
		{
			int ecode = tcrdbecode(rdb);
			if(ecode == TTENOREC) {
				dh->updateLoadUser(uid, user);
				return 1;
			}
			LOG4CXX_ERROR(logger_, "Get user ERROR : " << tcrdberrmsg(ecode));
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			if(type==1)
			{
				db_user_load[dbid] = NULL;
			}
			else if(type==2)
			{
				db_user_load2[dbid] = NULL;
			}
			return -1;
		}
	}
	
	LOG4CXX_DEBUG(logger_, "Loaded User: uid="<<uid);
	//time_t ltEnd = Clock::getCurrentSystemTime();
	//CMsg2QQDB::GetInstance()->TellLoadDB(int(ltEnd-ltStart),
	//	serverConfig.gamedIp(nid_).c_str(),
	//	serverConfig.dbUserAddr1(dbid).c_str(),
	//	serverConfig.dbUserPort1(dbid));

	//DB_Effect* pEffect = new DB_Effect();
	//pEffect->Clear();//不再读取effect
	//loadUserEffect(dh,pEffect,uid,2);

	//DB_NPCList* pNpcList = new DB_NPCList();
	//loadUserNPC(dh,pNpcList,uid);

	string data(buffer, len);
	user = new User();
	dh->acquireDataLock();
	LoadUserFromString(dh, user, data);//,pNpcList,pEffect);
	dh->releaseDataLock();
	dh->updateLoadUser(uid, user);
	free(buffer);
	return 0;
}


void GameDataSaver::routineUser(ActionType type)
{
	int ret = 0;
	int ret2= 0;
	int ret3= 0;
	pthread_mutex_t* mutex_ = (type == ACTION_USER_LOAD)?&load_user_mutex_:&save_user_mutex_;
	list<UserItem>* list = (type == ACTION_USER_LOAD)? &load_user_list_ : &save_user_list_;

	while(true)
	{
		pthread_mutex_lock(mutex_);
		if(type == ACTION_USER_SAVE)
		{
			int listlen = list->size();
			writeSaveListLength(listlen);
		}
		if(list->empty())
		{
			pthread_mutex_unlock(mutex_);
			if(b_serving)
			{
				usleep(500);
				continue;
			}
			else
			{
				break;
			}
		}
		UserItem item;
		if(!list->empty())
		{
			item = list->back();
			list->pop_back();
		}
		pthread_mutex_unlock(mutex_);

		if(type == ACTION_USER_SAVE)
		{
			ret3= saveUser2HotCache(item.dh, item.uid, *item.data);
			ret = saveUser(item.dh, item.uid, *item.data,1);

			saveUserNPC2HotCache(item.dh,item.uid,*item.npcdata);

		}
		else
		{
			ret = loadUser(item.dh, item.uid,1);
		}
		if(ret >= 0)
		{
			if(item.data != NULL)
			{
				delete item.data;
			}
			//if (item.effdata !=NULL)
			//{
			//	delete item.effdata;
			//}
			if (item.npcdata != NULL)
			{
				delete item.npcdata;
			}
		}
		else
		{
			// retry
			pthread_mutex_lock(mutex_);
			list->push_front(item);
			pthread_mutex_unlock(mutex_);
			usleep(500);
			continue;
		}
		usleep(500);
	}
}

void* GameDataSaver::saveUserThread(void* arg)
{
	GameDataSaver* ds = static_cast<GameDataSaver*>(arg);
	ds->routineUser(ACTION_USER_SAVE);
	pthread_exit(NULL);
	return NULL;
}

void* GameDataSaver::loadUserThread(void* arg)
{
	GameDataSaver* ds = static_cast<GameDataSaver*>(arg);
	ds->routineUser(ACTION_USER_LOAD);
	pthread_exit(NULL);
	return NULL;
}

void GameDataSaver::MakeUserString(GameDataHandler *dh, User *pUser, string &oString)//,string &oEffString,string &oNpcString )
{
	if (pUser == NULL)
	{
		return;
	}
	DB_User& dbUser = pUser->GetDbUser();
	dbUser.SerializeToString( &oString );

// 	DB_NPCList& dbNPCList = pUser->GetDBNPCList();
// 	dbNPCList.SerializeToString(&oNpcString);
// 	
// 	DB_Effect& dbEffect = pUser->GetDBEffect();
// 	dbEffect.SerializeToString(&oEffString);
}
/*
void    GameDataSaver::LoadUserEffectFromString(DB_Effect* pEffect,string &iString)
{
	if (pEffect == NULL)
	{
		return;
	}
	pEffect->ParseFromString(iString);
}

void   GameDataSaver::LoadUserNpcFromString(DB_NPCList* pNpcList, string &iString)
{
	if (pNpcList == NULL)
	{
		return;
	}
	pNpcList->ParseFromString(iString);
}
*/
void GameDataSaver::LoadUserFromString(GameDataHandler *dh, User *pUser, string &oString/*, DB_NPCList* pNpcList,DB_Effect* pEffect*/)
{
	if (pUser == NULL)
	{
		return;
	}

	DB_User dbUser;
	bool bSuc = dbUser.ParseFromString( oString );
	//pUser->SetDbEffect(pEffect);
	//pUser->SetDbNpcList(pNpcList);
	pUser->SetDbUser(dbUser);
}


//game DB相关
TCRDB* GameDataSaver::getUserLoadDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_user_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().GameMemDBAddr(dbid), HotCacheDBCfg::Instance().GameMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_user_load[dbid] = rdb;
	}
	return rdb;

}

TCRDB* GameDataSaver::getUserSaveDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_user_save[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().GameMemDBAddr(dbid), HotCacheDBCfg::Instance().GameMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_user_save[dbid] = rdb;
	}
	return rdb;

}

int GameDataSaver::saveUser2HotCache(GameDataHandler* const dh, int64& uid, string& data)
{
	if(!m_bHotCacheEnable)
		return 0;

	int dbid = getUidHash(uid) % db_num + 1;
	TCRDB* rdb = getUserSaveDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return -1;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, suid, 16, buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put user 2 HotCache ERROR : uid:" <<uid <<",valuelength:"<< len <<",error:"<< tcrdberrmsg(ecode));

		writeSaveError(tcrdberrmsg(ecode),suid,len);

		tcrdbclose(rdb);
		tcrdbdel(rdb);
		hotcache_user_save[dbid] = NULL;

		return -1;
	}
	time_t ltEnd = Clock::getCurrentSystemTime();

	LOG4CXX_DEBUG(logger_, "Saved User 2 HotCache: uid="<<uid);
	return 0;
}

int GameDataSaver::saveUserNPC2HotCache(GameDataHandler* const dh, int64& uid, string& data)
{
	if(!m_bHotCacheEnable)
		return 0;

	int dbid = getUidHash(uid) % db_num + 1;
	TCRDB* rdb = getUserSaveDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return -1;
	}
	std::string key = "NPC_" + toString(uid) ;
	int klen = key.length();
	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, key.c_str(), klen, buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put user NPC 2 HotCache ERROR : uid:" <<uid <<",valuelength:"<< len << ",error:"<< tcrdberrmsg(ecode));

		writeSaveError(tcrdberrmsg(ecode),key.c_str(),len);

		tcrdbclose(rdb);
		tcrdbdel(rdb);
		hotcache_user_save[dbid] = NULL;

		return -1;
	}
	LOG4CXX_DEBUG(logger_, "Saved UserNPC 2 HotCache: uid="<<uid);
	return 0;
}

char* GameDataSaver::loadUserFromHotCache(GameDataHandler* const dh, int64& uid, int& len)
{
	if(!m_bHotCacheEnable||!m_bHotCacheEnableLoad)
		return NULL;

	int dbid = getUidHash(uid) % db_num + 1;

	TCRDB* rdb = getUserLoadDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return NULL;
	}
	char suid[32];
	sprintf(suid, "%016llX", uid);

	User* user = NULL;
	char* buffer = (char*)tcrdbget(rdb, suid, 16, &len);
	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode != TTENOREC) {
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_user_load[dbid] = NULL;
		}
		
	}
	LOG4CXX_DEBUG(logger_, "Loaded User: uid="<<uid);
	return buffer;
}

char* GameDataSaver::loadUserNPCFromHotCache(GameDataHandler* const dh, int64& uid, int& len)
{
	if(!m_bHotCacheEnable||!m_bHotCacheEnableLoadNPC)
		return NULL;

	int dbid = getUidHash(uid) % db_num + 1;

	TCRDB* rdb = getUserLoadDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return NULL;
	}

	std::string key = "NPC_" + toString(uid) ;
	int klen = key.length();

	char* buffer = (char*)tcrdbget(rdb, key.c_str(), klen, &len);
	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode != TTENOREC) {
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_user_load[dbid] = NULL;
		}
	}
	return buffer;
}

