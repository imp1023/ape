#define _CRT_SECURE_NO_WARNINGS
#include "../logic/dbinterface.pb.h"
#include "PlatDataSaver.h"
#include <iomanip>
#include "PlatDataHandler.h"
#include <sys/types.h>
#include "../common/Clock.h"
#include "../common/HotCacheDBCfg.h"
#include "../common/distribution.h"

#include "../gamed/MemCacheServerHandler.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

extern ServerConfig serverConfig;

PlatDataSaver::PlatDataSaver(int nid)
{
	nid_ = nid;
	db_num = serverConfig.dbNum();
	sprintf(allocate_file, "allocate_g%d.cfg", nid_);
	sprintf(allocate_dbstr,"ALLOCATE_G%05d",nid_);
	logger_ = log4cxx::Logger::getLogger("PlatDataSaver");
	alloc_rdb = NULL;
	alloc_rdb2= NULL;
	//plat_tmp_rdb = NULL;
	//user_tmp_rdb = NULL;
	log_plattime = 0;
	count_dirty_plat_ = 0;

	initDatabase();

	pthread_mutex_init(&load_map_mutex_, NULL);
	pthread_mutex_init(&save_map_mutex_, NULL);

	b_serving = true;

	m_bHotCacheEnable		= HotCacheDBCfg::Instance().EnableWrite();
	//如果不可写，则不可读
	m_bHotCacheEnableLoad	= HotCacheDBCfg::Instance().EnableRead();
	m_bHotCacheEnableLoadNPC= HotCacheDBCfg::Instance().EnableReadNPC();
	initThreads();
}

PlatDataSaver::~PlatDataSaver()
{
	termThreads();

	pthread_mutex_destroy(&load_map_mutex_);
	pthread_mutex_destroy(&save_map_mutex_);
}

void PlatDataSaver::initThreads()
{
	int ret = 0;

	ret = pthread_create(&th_map_load, NULL, PlatDataSaver::loadMapThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating load map thread");
	}

	ret = pthread_create(&th_map_save, NULL, PlatDataSaver::saveMapThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating save thread");
	}
}

void PlatDataSaver::termThreads()
{
	b_serving = false;
	pthread_join(th_map_load, NULL);
	pthread_join(th_map_save, NULL);

	LOG4CXX_ERROR(logger_, "Threads terminated. Quit.");
	termDatabase();
}

void PlatDataSaver::initDatabase()
{
	void * tmp = NULL;
	for(int i = 0; i < db_num; i ++)
	{
		db_plat_load.insert(make_pair(i + 1, tmp));
		db_plat_save.insert(make_pair(i + 1, tmp));

		db_plat_load2.insert(make_pair(i + 1, tmp));
		db_plat_save2.insert(make_pair(i + 1, tmp));

		hotcache_plat_load.insert(make_pair(i + 1, tmp));
		hotcache_plat_save.insert(make_pair(i + 1, tmp));

	}
}

void PlatDataSaver::termDatabase()
{
	for(int i = 0; i < db_num; i ++)
	{
		TCRDB* rdb = NULL;

		rdb = (TCRDB*)db_plat_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_plat_load[i] = NULL;
		}

		rdb = (TCRDB*)db_plat_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_plat_save[i] = NULL;
		}

		rdb = (TCRDB*)db_plat_load2[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_plat_load2[i] = NULL;
		}

		rdb = (TCRDB*)db_plat_save2[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_plat_save2[i] = NULL;
		}

		rdb = (TCRDB*)hotcache_plat_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_plat_load[i] = NULL;
		}

		rdb = (TCRDB*)hotcache_plat_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_plat_save[i] = NULL;
		}
	}
	
	db_plat_load2.clear();
	db_plat_save2.clear();

	db_plat_load.clear();
	db_plat_save.clear();

	hotcache_plat_load.clear();
	hotcache_plat_save.clear();


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


	//if(plat_tmp_rdb != NULL)
	//{
	//	tcrdbclose(plat_tmp_rdb);
	//	tcrdbdel(plat_tmp_rdb);
	//	plat_tmp_rdb = NULL;
	//}
	//if(user_tmp_rdb != NULL)
	//{
	//	tcrdbclose(user_tmp_rdb);
	//	tcrdbdel(user_tmp_rdb);
	//	user_tmp_rdb = NULL;

	//}
}

TCRDB* PlatDataSaver::getAllocateDb(int type)
{
	if(type == 1)
	{
		TCRDB* rdb = tcrdbnew();
		if (!tcrdbopen(rdb, serverConfig.dbUserAddr1(1).c_str(), serverConfig.dbUserPort1(1)))
		{
			LOG4CXX_ERROR(logger_, "open db[" << serverConfig.dbUserAddr1(1)
				<< ":" << serverConfig.dbUserPort1(1)
				<<"] error: " << tcrdberrmsg(tcrdbecode(rdb)));
			tcrdbdel(rdb);
			rdb = NULL;
		}
		return rdb;
	}
	else if(type==2)
	{
		TCRDB* rdb = tcrdbnew();
		if (!tcrdbopen(rdb, serverConfig.dbUserAddr2(1).c_str(), serverConfig.dbUserPort2(1)))
		{
			LOG4CXX_ERROR(logger_, "open db[" << serverConfig.dbUserAddr2(1)
				<< ":" << serverConfig.dbUserPort2(1)
				<<"] error: " << tcrdberrmsg(tcrdbecode(rdb)));
			tcrdbdel(rdb);
			rdb = NULL;
		}
		return rdb;
	}

	return NULL;
}



void PlatDataSaver::loadAllocateSetting(int64& uid, int64& iid)
{
	if(alloc_rdb == NULL)
	{
		alloc_rdb = getAllocateDb(1);
	}
	char* buffer = NULL;
	if(alloc_rdb != NULL)
	{
		buffer = (char*)tcrdbget2(alloc_rdb, allocate_dbstr);
	}
	if(buffer != NULL)
	{
		sscanf(buffer, "%lld,%lld", &uid, &iid);
		free(buffer);
	}
	else
	{
		//if not in db, read info from file,this is use for change info from file to db,will delete later.
		FILE *fp = fopen(allocate_file, "r");
		if(NULL != fp)
		{
			char setting[256];
			fgets(setting, 256, fp);
			if(strlen(setting) >= 7)
			{
				sscanf(setting, "%lld,%lld", &uid, &iid);
			}
			fclose(fp);
		}
	}
}

void PlatDataSaver::saveAllocateSetting(int64& uid, int64& iid)
{
	if(alloc_rdb == NULL)
	{
		alloc_rdb = getAllocateDb(1);
	}
	if(alloc_rdb != NULL)
	{
		char setting[256];
		sprintf(setting, "%lld,%lld", uid, iid);
		if(!tcrdbput2(alloc_rdb, allocate_dbstr, setting))
		{
			LOG4CXX_ERROR(logger_, "Put allocate ERROR : " << tcrdberrmsg(tcrdbecode(alloc_rdb)));
			tcrdbclose(alloc_rdb);
			tcrdbdel(alloc_rdb);
			alloc_rdb = NULL;
		}
	}

	//if(alloc_rdb2 == NULL)
	//{
	//	alloc_rdb2 = getAllocateDb(2);
	//}
	//if(alloc_rdb2 != NULL)
	//{
	//	char setting[256];
	//	sprintf(setting, "%lld,%lld", uid, iid);
	//	if(!tcrdbput2(alloc_rdb2, allocate_dbstr, setting))
	//	{
	//		LOG4CXX_ERROR(logger_, "Put allocate ERROR : " << tcrdberrmsg(tcrdbecode(alloc_rdb2)));
	//		tcrdbclose(alloc_rdb2);
	//		tcrdbdel(alloc_rdb2);
	//		alloc_rdb2 = NULL;
	//	}
	//}

	FILE *fp = fopen(allocate_file, "w");
	if(NULL != fp)
	{
		fprintf(fp, "%lld,%lld", uid, iid);
		fclose(fp);
	}
}

void PlatDataSaver::writePlatSaveListLength(int len)
{
	time_t now = time(NULL);
	if(now != log_plattime && now % 10 == 0)
	{
		log_plattime = now;
		char logfile[256];
		sprintf(logfile,"savelist_plat%d.log",nid_);
		FILE *fp = fopen(logfile, "a");
		if(NULL != fp)
		{
			struct tm *tmnow = NULL;
			char szTime[128];
			tmnow = localtime(&now);
			strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", tmnow);
			fprintf(fp, "%s -  list cnt:%d , dirty cnt: %d  \n", szTime, len, count_dirty_plat_);
			fclose(fp);
		}
	}
}

void PlatDataSaver::writeSaveError(const char *strError,const char* szKey,int nValueLen)
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

int PlatDataSaver::safeSaveAllMap(time_t revision, PlatDataHandler* const dh, bool force/* =false */)
{
	// map data
	pthread_mutex_lock(&save_map_mutex_);
	map<string, DB_Plat*> &list = dh->dirty_platlst_;
	map<string, DB_Plat*>::iterator iter = list.begin();
	//hash_map<string, int64>::iterator itRes;
	//hash_map<string, int64> &mapRevision = dh->dirty_platlst_revision_;
	int list_len = save_map_list_.size();
	int save_interval = 1;
	if(list_len > 250)
	{
		save_interval += (list_len - 250) / 50;
		if(save_interval > 20)
		{
			save_interval = 20;
		}
	}
	while(iter!=list.end())
	{
		map<string, DB_Plat*>::iterator oiter = iter;
		++iter;
		const string& pid = oiter->first;
		DB_Plat* pDBPlat = oiter->second;
		int64 nRevision = pDBPlat->ltsaverevision();
		//itRes = mapRevision.find(pid);
		//if(itRes !=mapRevision.end())
		//{
		//	nRevision = itRes->second;
		//}
		if(force || nRevision < revision - save_interval * 60 * 1000)
		{
			pushSaveMap(dh, pDBPlat);
			list.erase(oiter);
			//if(itRes != mapRevision.end())
			//{
			//	mapRevision.erase(itRes);
			//}
		}
	}
	count_dirty_plat_ = list.size();

	pthread_mutex_unlock(&save_map_mutex_);
	LOG4CXX_DEBUG(logger_, "All PlatformID->UID Map saved.");
	return 0;
}

int PlatDataSaver::safeSaveMap(PlatDataHandler* const dh, DB_Plat *pDBPlat)
{
	if(pDBPlat)
	{
		pthread_mutex_lock(&save_map_mutex_);
		pushSaveMap(dh, pDBPlat);
		pthread_mutex_unlock(&save_map_mutex_);
		LOG4CXX_DEBUG(logger_, "Save platid map: platid="<<pDBPlat->platform_id());
	}
	return 0;
}

int PlatDataSaver::safeLoadMap(PlatDataHandler* const dh, const string& pid)
{
	pthread_mutex_lock(&load_map_mutex_);
	pushLoadMap(dh, pid);
	pthread_mutex_unlock(&load_map_mutex_);
	LOG4CXX_DEBUG(logger_, "Load user map " << pid);
	return 0;
}

int PlatDataSaver::pushSaveMap(PlatDataHandler* const dh,  DB_Plat* pDBPlat)
{
	// 一个openid对应一个uid列表，openid已存在，追加一个
	// openid不存在，追加一个新项，uid为列表的第一项
	if(!pDBPlat->has_platform_id()||pDBPlat->platform_id().size()<2)
		return 0;
	MapItem save_data;
	save_data.dh = dh;
	save_data.platid = pDBPlat->platform_id();

	string * data = new string;
	MakePlatidString(dh,pDBPlat,*data);
	save_data.data = data;

	save_map_list_.push_front(save_data);

	return 0;
}

int PlatDataSaver::pushLoadMap(PlatDataHandler* const dh, const string& platid)
{
	MapItem load_data;
	load_data.dh = dh;
	load_data.platid = platid;
	load_data.data =NULL;

	load_map_list_.push_front(load_data);
	return 0;
}

TCRDB* PlatDataSaver::getDb(const string& addr, int port)
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


TCRDB* PlatDataSaver::getPlatLoadDb(int dbid,int type)
{
	if(type == 1)
	{
		TCRDB *rdb = (TCRDB *)db_plat_load[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbPlatAddr1(dbid), serverConfig.dbPlatPort1(dbid));
		}
		if(rdb != NULL)
		{
			db_plat_load[dbid] = rdb;
		}
		return rdb;
	}
	else if(type == 2)
	{
		TCRDB *rdb = (TCRDB *)db_plat_load2[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbPlatAddr2(dbid), serverConfig.dbPlatPort2(dbid));
		}
		if(rdb != NULL)
		{
			db_plat_load2[dbid] = rdb;
		}
		return rdb;
	}
	return NULL;

}

TCRDB* PlatDataSaver::getPlatSaveDb(int dbid,int type)
{
	if(type == 1)
	{
		TCRDB *rdb = (TCRDB *)db_plat_save[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbPlatAddr1(dbid), serverConfig.dbPlatPort1(dbid));
		}
		if(rdb != NULL)
		{
			db_plat_save[dbid] = rdb;
		}
		return rdb;
	}
	else if(type == 2)
	{
		TCRDB *rdb = (TCRDB *)db_plat_save2[dbid];
		if(rdb == NULL)
		{
			rdb = getDb(serverConfig.dbPlatAddr2(dbid), serverConfig.dbPlatPort2(dbid));
		}
		if(rdb != NULL)
		{
			db_plat_save2[dbid] = rdb;
		}
		return rdb;
	}
	return NULL;

}



int PlatDataSaver::saveMap(PlatDataHandler* const dh, const string& pid, string& data,int type)
{
	int dbid = getPlatidHash(pid) % db_num + 1;
	TCRDB* rdb = getPlatSaveDb(dbid,type);
	if(rdb == NULL)
	{
		return -1;
	}

	//char suid[32];
	//sprintf(suid, "%016llX", uid);
	//if(!tcrdbput2(rdb, pid.c_str(), suid))
	//{
	//	int ecode = tcrdbecode(rdb);
	//	LOG4CXX_ERROR(logger_, "Put map ERROR : " << tcrdberrmsg(ecode));
	//	tcrdbclose(rdb);
	//	tcrdbdel(rdb);
	//	if(type==1)
	//	{
	//		db_plat_save[dbid] = NULL;
	//	}
	//	else if(type==2)
	//	{
	//		db_plat_save2[dbid] = NULL;
	//	}
	//	
	//	return -1;
	//}

	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, pid.c_str(), pid.size(), buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_plat_save[dbid] = NULL;
		return -1;
	}
	LOG4CXX_DEBUG(logger_, "Saved Map:platid="<<pid);
	return 0;
}


int PlatDataSaver::loadMap(PlatDataHandler* const dh, const string& platid,int type)
{
	int dbid = getPlatidHash(platid) % db_num + 1;

	int len = 0;
	int64 uid = 0;
	char* buffer = (char*) loadMapFromHotCache(dh,platid,len);
	if(buffer==NULL)
	{
		TCRDB* rdb = getPlatLoadDb(dbid,type);
		if(rdb == NULL)
		{
			return -1;
		}
		int klen = platid.length();
		buffer = (char*)tcrdbget(rdb, platid.c_str(), klen, &len);
		if(buffer == NULL)
		{
			int ecode = tcrdbecode(rdb);
			if(ecode == TTENOREC) {
				dh->updateLoadPlatid(platid, NULL);
				return 1;
			}
			LOG4CXX_ERROR(logger_, "Get map ERROR : " << tcrdberrmsg(ecode));
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			if(type==1)
			{
				db_plat_load[dbid] = NULL;
			}
			else if(type==2)
			{
				db_plat_load2[dbid] = NULL;
			}
			return -1;
		}
	}
	
	DB_Plat* pDBPlat = new DB_Plat();

	if (NULL == strpbrk(buffer, "new:"))
	{
		sscanf(buffer, "%016llX", &uid);
		DB_PlatRegion* pPlatidInfo = pDBPlat->add_platregion();
		if (pPlatidInfo)
		{
			pPlatidInfo->set_uid(uid);
			pPlatidInfo->set_srvregion(0);
		}
		pDBPlat->set_platform_id(platid);
		pDBPlat->set_lastregion(0);
		//pDBPlat->set_bandcoin(0);
	}
	else
	{
		string data(buffer, len);
		data = data.substr(4);
		dh->acquireDataLock();
		LoadPlatidFromString(dh,pDBPlat,data);
		dh->releaseDataLock();
	}
	dh->updateLoadPlatid(platid,pDBPlat);
	LOG4CXX_DEBUG(logger_, "Loaded Map:platid="<<platid);
	free(buffer);

	return 0;
}

void PlatDataSaver::routineMap(ActionType type)
{
	int ret = 0;
	int ret2= 0;
	int ret3= 0;
	pthread_mutex_t* mutex_ = (type == ACTION_MAP_LOAD)?&load_map_mutex_:&save_map_mutex_;
	list<MapItem>* list = (type == ACTION_MAP_LOAD)? &load_map_list_ : &save_map_list_;

	while(true)
	{
		pthread_mutex_lock(mutex_);
		if(type == ACTION_MAP_SAVE)
		{
			int listlen = list->size();
			writePlatSaveListLength(listlen);
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

		MapItem item;
		if(!list->empty())
		{
			item = list->back();
			list->pop_back();
		}
		pthread_mutex_unlock(mutex_);

		if(type == ACTION_MAP_SAVE)
		{
			ret = saveMap(item.dh, item.platid, *item.data,1);
			//下个版本物理数据库将不再有备库
			//ret2= saveMap(item.dh, item.platid, *item.data,2);
			ret3= saveMap2HotCache(item.dh, item.platid, *item.data);
		}
		else
		{
			ret = loadMap(item.dh, item.platid,1);
		}
		if(ret >= 0)
		{
			if(item.data != NULL)
			{
				delete item.data;
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



void* PlatDataSaver::saveMapThread(void* arg)
{
	PlatDataSaver* ds = static_cast<PlatDataSaver*>(arg);
	ds->routineMap(ACTION_MAP_SAVE);
	pthread_exit(NULL);
	return NULL;
}

void* PlatDataSaver::loadMapThread(void* arg)
{
	PlatDataSaver* ds = static_cast<PlatDataSaver*>(arg);
	ds->routineMap(ACTION_MAP_LOAD);
	pthread_exit(NULL);
	return NULL;
}

// platid信息序列化为字符串
void PlatDataSaver::MakePlatidString(PlatDataHandler *dh, DB_Plat *pDBPlat, string &oString )
{
	if (pDBPlat == NULL)
	{
		return;
	}
	pDBPlat->SerializeToString( &oString );
	oString = "new:" + oString;
}
void PlatDataSaver::LoadPlatidFromString(PlatDataHandler *dh, DB_Plat *pDBPlat, string &iString)
{
	if (pDBPlat == NULL)
	{
		return;
	}
	pDBPlat->ParseFromString( iString );
}

//game DB相关

TCRDB* PlatDataSaver::getPlatLoadDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_plat_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().PlatMemDBAddr(dbid), HotCacheDBCfg::Instance().PlatMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_plat_load[dbid] = rdb;
	}
	return rdb;
}

TCRDB* PlatDataSaver::getPlatSaveDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_plat_save[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().PlatMemDBAddr(dbid), HotCacheDBCfg::Instance().PlatMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_plat_save[dbid] = rdb;
	}
	return rdb;
}

int PlatDataSaver::saveMap2HotCache(PlatDataHandler* const dh, const string& pid, string& data)
{
	if(!m_bHotCacheEnable)
	{
		return 0;
	}
	int dbid = getPlatidHash(pid) % db_num + 1;
	TCRDB* rdb = getPlatSaveDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return -1;
	}

	//char suid[32];
	//sprintf(suid, "%016llX", uid);
	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, pid.c_str(), pid.size(), buf, len))
	//if(!tcrdbput2(rdb, pid.c_str(), suid))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		hotcache_plat_save[dbid] = NULL;
		return -1;
	}
	//LOG4CXX_DEBUG(logger_, "Saved Map 2 HotCache:platid="<<pid<<"->uid="<<uid);
	return 0;
}

char* PlatDataSaver::loadMapFromHotCache(PlatDataHandler* const dh, const string& platid,int& len)
{
	if(!m_bHotCacheEnable||!m_bHotCacheEnableLoad)
	{
		return NULL;
	}

	int dbid = getPlatidHash(platid) % db_num + 1;

	TCRDB* rdb = getPlatLoadDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return NULL;
	}

	int64 uid = 0;
	//char* buffer = (char*)tcrdbget2(rdb, platid.c_str());
	int klen = platid.length();
	char* buffer = (char*)tcrdbget(rdb, platid.c_str(), klen, &len);

	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode != TTENOREC) {
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_plat_load[dbid] = NULL;
		}
	}
	return buffer;

}
