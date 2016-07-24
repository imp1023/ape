#define _CRT_SECURE_NO_WARNINGS
#include "../logic/dbinterface.pb.h"
#include "CountryDataSaver.h"
#include <iomanip>
#include "CountryDataHandler.h"
#include <sys/types.h>
#include "../common/Clock.h"
#include "../common/HotCacheDBCfg.h"
#include "../common/distribution.h"

//#include "../gamed/MemCacheServerHandler.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

extern ServerConfig serverConfig;

CountryDataSaver::CountryDataSaver(int nid)
{
	nid_ = nid;
	db_num = serverConfig.countryDBNum();

	logger_ = log4cxx::Logger::getLogger("CountryDataSaver");

	log_plattime = 0;
	m_nDirtyCountryCnt = 0;
	initDatabase();

	pthread_mutex_init(&load_map_mutex_, NULL);
	pthread_mutex_init(&save_map_mutex_, NULL);

	b_serving = true;

	m_bHotCacheEnable		= HotCacheDBCfg::Instance().EnableCountryDB();

	initThreads();
}

CountryDataSaver::~CountryDataSaver()
{
	termThreads();

	pthread_mutex_destroy(&load_map_mutex_);
	pthread_mutex_destroy(&save_map_mutex_);
}

void CountryDataSaver::initThreads()
{
	int ret = 0;
	ret = pthread_create(&th_map_save, NULL, CountryDataSaver::saveMapThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating save thread");
	}
}

void CountryDataSaver::termThreads()
{
	b_serving = false;
	//pthread_join(th_map_load, NULL);
	pthread_join(th_map_save, NULL);

	LOG4CXX_ERROR(logger_, "Threads terminated. Quit.");
	termDatabase();
}

void CountryDataSaver::initDatabase()
{
	void * tmp = NULL;
	for(int i = 0; i < db_num; i ++)
	{
		db_country_load.insert(make_pair(i + 1, tmp));
		db_country_save.insert(make_pair(i + 1, tmp));
		hotcache_country_load.insert(make_pair(i + 1, tmp));
		hotcache_country_save.insert(make_pair(i + 1, tmp));
	}
}

void CountryDataSaver::termDatabase()
{
	for(int i = 0; i < db_num; i ++)
	{
		TCRDB* rdb = NULL;

		rdb = (TCRDB*)db_country_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_country_load[i] = NULL;
		}

		rdb = (TCRDB*)db_country_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_country_save[i] = NULL;
		}


		rdb = (TCRDB*)hotcache_country_load[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_country_load[i] = NULL;
		}

		rdb = (TCRDB*)hotcache_country_save[i];
		if(rdb != NULL)
		{
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_country_save[i] = NULL;
		}
	}
	

	db_country_load.clear();
	db_country_save.clear();

	hotcache_country_load.clear();
	hotcache_country_save.clear();



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

int CountryDataSaver::safeSaveAll(time_t revision, CountryDataHandler* const dh, bool force/* =false */)
{
	// map data
	safeSaveAllCountry(revision,dh,force);
	return 0;
}

int CountryDataSaver::safeSaveAllCountry(time_t revision, CountryDataHandler* const dh, bool force)
{
	pthread_mutex_lock(&save_map_mutex_);

	map<int, CCountry*> &list = dh->m_mapDirtyCountry;
	map<int, CCountry*>::iterator iter = list.begin();
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
		map<int, CCountry*>::iterator oiter = iter;
		++iter;

		CCountry* pCountry = oiter->second;
		DB_C_Country* pDBCountry = &(pCountry->m_dbCountry);
		int64 nRevision = pDBCountry->ltsaverevision();

		if(force || nRevision < revision - save_interval * 60 * 1000)
		{
			pushSaveCountry(dh, pCountry->GetKey(), pDBCountry);
			list.erase(oiter);
		}
	}
	m_nDirtyCountryCnt = list.size();

	pthread_mutex_unlock(&save_map_mutex_);

	LOG4CXX_DEBUG(logger_, "All Country Map saved.");
	return 0;
}

int CountryDataSaver::pushSaveCountry(CountryDataHandler* const dh, int nKey,DB_C_Country* pDBCountry)
{
	CityDBItem save_data;
	char szKey[256];
	sprintf(szKey,"Country_%d",nKey);

	save_data.dh = dh;
	save_data.key = szKey;

	string * data = new string;
	pDBCountry->SerializeToString( data );
	save_data.data = data;

	save_map_list_.push_front(save_data);

	return 0;
}

TCRDB* CountryDataSaver::getDb(const string& addr, int port)
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

TCRDB* CountryDataSaver::getCountryLoadDb(int dbid)
{
	if(dbid<=0)
		return NULL;
	TCRDB *rdb = (TCRDB *)db_country_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(serverConfig.dbCountryAddr(dbid), serverConfig.dbCountryPort(dbid));
	}
	if(rdb != NULL)
	{
		db_country_load[dbid] = rdb;
	}
	return rdb;
}

TCRDB* CountryDataSaver::getCountrySaveDb(int dbid)
{
	if(dbid<=0)
		return NULL;
	TCRDB *rdb = (TCRDB *)db_country_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(serverConfig.dbCountryAddr(dbid), serverConfig.dbCountryPort(dbid));
	}
	if(rdb != NULL)
	{
		db_country_load[dbid] = rdb;
	}
	return rdb;
}
 

int CountryDataSaver::GetDBIDByKey(string strKey)
{
	int nPos = strKey.find("_");
	if(nPos<=0)
	{
		LOG4CXX_ERROR(logger_, "Get DBID ERROR : " <<strKey << "!!!!!!!!!!!!!");
		return -1;
	}
	string strSub = strKey.substr(nPos+1);
	int nID = atoi(strSub.c_str());
	//nID /= MAX_CITY_ID;
	nID%=serverConfig.countryDBNum();
	nID += 1;

	return nID;
}

int CountryDataSaver::saveInfo2DB(CountryDataHandler* const dh, string strKey, string& data)
{
	int dbid = GetDBIDByKey(strKey);
	TCRDB* rdb = getCountrySaveDb(dbid);
	if(rdb == NULL)
	{
		return -1;
	}

	int nKeyLen = strKey.size();

	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, strKey.c_str(), nKeyLen, buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_country_save[dbid] = NULL;
		return -1;
	}
	LOG4CXX_DEBUG(logger_, "Saved Map:platid="<<strKey);
	return 0;
}


int CountryDataSaver::loadCountry(const string& strKey,DB_C_Country& dbCountry)
{
	int dbid = GetDBIDByKey(strKey);

	int len = 0;
	int64 uid = 0;
	char* buffer = (char*) loadInfoFromHotCache(strKey,len);
	if(buffer==NULL)
	{
		TCRDB* rdb = getCountryLoadDb(dbid);
		if(rdb == NULL)
		{
			return -1;
		}
		int klen = strKey.length();
		buffer = (char*)tcrdbget(rdb, strKey.c_str(), klen, &len);
		if(buffer == NULL)
		{
			int ecode = tcrdbecode(rdb);
			if(ecode == TTENOREC) {
				return 1;
			}
			LOG4CXX_ERROR(logger_, "Get map ERROR : " << tcrdberrmsg(ecode));
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_country_load[dbid] = NULL;

			return -1;
		}
	}
	string data(buffer, len);
	dbCountry.ParseFromString(data);
	free(buffer);	
	return 0;
}

void CountryDataSaver::writeCountrySaveListLength(int len)
{
	time_t now = time(NULL);
	if(now != log_plattime && now % 10 == 0)
	{
		log_plattime = now;
		char logfile[256];
		sprintf(logfile,"savelist_country%d.log",nid_);
		FILE *fp = fopen(logfile, "a");
		if(NULL != fp)
		{
			struct tm *tmnow = NULL;
			char szTime[128];
			tmnow = localtime(&now);
			strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", tmnow);
			fprintf(fp, "%s - %d dirtyCity:%d,dirtyCountry:%d,dirtyUserLst:%d,dirtyGuildLst:%d\n", szTime, len, 0,m_nDirtyCountryCnt,0,0);
			fclose(fp);
		}
	}
}

void CountryDataSaver::writeSaveError(const char *strError,const char* szKey,int nValueLen)
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

void CountryDataSaver::routine(ActionType type)
{
	int ret = 0;
	int ret2= 0;
	int ret3= 0;
	pthread_mutex_t* mutex_ = (type == ACTION_COUNTRY_LOAD)?&load_map_mutex_:&save_map_mutex_;
	list<CityDBItem>* list = (type == ACTION_COUNTRY_LOAD)? &load_map_list_ : &save_map_list_;

	while(true)
	{
		pthread_mutex_lock(mutex_);
		if(list->empty())
		{
			pthread_mutex_unlock(mutex_);
			if(type == ACTION_COUNTRY_SAVE)
			{
				int listlen = list->size();
				writeCountrySaveListLength(listlen);
			}
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

		CityDBItem item;
		if(!list->empty())
		{
			item = list->back();
			list->pop_back();
		}
		pthread_mutex_unlock(mutex_);

		if(type == ACTION_COUNTRY_SAVE)
		{
			ret = saveInfo2DB(item.dh, item.key, *item.data);
			
			ret3= saveInfo2HotCache(item.dh, item.key, *item.data);
		}
		//else
		//{
		//	ret = loadCity(item.dh, item.key);
		//}
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


void* CountryDataSaver::saveMapThread(void* arg)
{
	CountryDataSaver* ds = static_cast<CountryDataSaver*>(arg);
	ds->routine(ACTION_COUNTRY_SAVE);
	pthread_exit(NULL);
	return NULL;
}

void* CountryDataSaver::loadMapThread(void* arg)
{
	CountryDataSaver* ds = static_cast<CountryDataSaver*>(arg);
	ds->routine(ACTION_COUNTRY_LOAD);
	pthread_exit(NULL);
	return NULL;
}


//void CountryDataSaver::LoadCountryidFromString(CountryDataHandler *dh, DB_C_City *pDBCity, string &iString)
//{
//	if (pDBCity == NULL)
//	{
//		return;
//	}
//	pDBCity->ParseFromString( iString );
//}

//game DBœ‡πÿ

TCRDB* CountryDataSaver::getCountryLoadDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_country_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().CountryMemDBAddr(dbid), HotCacheDBCfg::Instance().CountryMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_country_load[dbid] = rdb;
	}
	return rdb;
}

TCRDB* CountryDataSaver::getCountrySaveDb4HotCache(int dbid)
{
	TCRDB *rdb = (TCRDB *)hotcache_country_save[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(HotCacheDBCfg::Instance().CountryMemDBAddr(dbid), HotCacheDBCfg::Instance().CountryMemDBPort(dbid));
	}
	if(rdb != NULL)
	{
		hotcache_country_save[dbid] = rdb;
	}
	return rdb;
}

int CountryDataSaver::saveInfo2HotCache(CountryDataHandler* const dh,string strKey , string& data)
{
	if(!m_bHotCacheEnable)
	{
		return 0;
	}
	int dbid = GetDBIDByKey(strKey);
	TCRDB* rdb = getCountrySaveDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return -1;
	}

	int nKeyLen = strKey.size();

	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, strKey.c_str() , nKeyLen , buf, len))
	//if(!tcrdbput2(rdb, pid.c_str(), suid))
	{
		int ecode = tcrdbecode(rdb);
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		hotcache_country_save[dbid] = NULL;
		return -1;
	}
	//LOG4CXX_DEBUG(logger_, "Saved Map 2 HotCache:platid="<<pid<<"->uid="<<uid);
	return 0;
}

char* CountryDataSaver::loadInfoFromHotCache(const string& strKey,int& len)
{
	if(!m_bHotCacheEnable)
	{
		return NULL;
	}

	int dbid = GetDBIDByKey(strKey);


	TCRDB* rdb = getCountryLoadDb4HotCache(dbid);
	if(rdb == NULL)
	{
		return NULL;
	}

	int64 uid = 0;
	//char* buffer = (char*)tcrdbget2(rdb, platid.c_str());
	int klen = strKey.length();
	char* buffer = (char*)tcrdbget(rdb, strKey.c_str(), klen, &len);

	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode != TTENOREC) {
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			hotcache_country_load[dbid] = NULL;
		}
	}
	return buffer;

}
