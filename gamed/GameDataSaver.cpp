#define _CRT_SECURE_NO_WARNINGS
#include "../logic/dbinterface.pb.h"
#include "GameDataSaver.h"
#include "GameDataHandler.h"
#include "../common/Clock.h"
#include "../common/distribution.h"
#include "../logic/GameServerConfig.h"
#include "../logic/User.h"
#include "../logic/StarLogic.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif


GameDataSaver::GameDataSaver(int nid)
{
	nid_ = nid;
	db_num = ServerConfig::Instance().dbNum();
    sprintf(allocate_file, "allocate_g%d.cfg", ServerConfig::Instance().gameregion(nid_));
	sprintf(allocate_dbstr,"ALLOCATE_G%05d",ServerConfig::Instance().gameregion(nid_));
	sprintf(GlobalStarInfo_DB, "GlobalStarInfo_G%05d", ServerConfig::Instance().gameregion(nid_));
	logger_ = log4cxx::Logger::getLogger("GameDataSaver");
	alloc_rdb = NULL;
	star_rdb = NULL;
	log_time = 0;
    log_error_time = 0;
	count_dirty_ = 0;

	initDatabase();

	pthread_mutex_init(&m_mutexLoadUser, NULL);
	pthread_mutex_init(&m_mutexSaveUser, NULL);
	pthread_mutex_init(&m_mutexLoadMap, NULL);
	pthread_mutex_init(&m_mutexSaveMap, NULL);

	b_serving = true;
	initThreads();
}

GameDataSaver::~GameDataSaver()
{
	termThreads();

	pthread_mutex_destroy(&m_mutexLoadUser);
	pthread_mutex_destroy(&m_mutexSaveUser);
	pthread_mutex_destroy(&m_mutexLoadMap);
	pthread_mutex_destroy(&m_mutexSaveMap);
}

void GameDataSaver::initThreads()
{
	int ret = 0;
	ret = pthread_create(&m_threadLoadUser, NULL, GameDataSaver::loadUserThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating load data thread");
	}
	ret = pthread_create(&m_threadSaveUser, NULL, GameDataSaver::saveUserThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating save data thread");
	}

	ret = pthread_create(&m_threadLoadMap, NULL, GameDataSaver::loadMapThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating load map thread");
	}

	ret = pthread_create(&m_threadSaveMap, NULL, GameDataSaver::saveMapThread, (void *)this);
	if(ret != 0)
	{
		LOG4CXX_ERROR(logger_, "ERROR creating save thread");
	}
}

void GameDataSaver::termThreads()
{
	if (b_serving)
	{
		b_serving = false;
		pthread_join(m_threadLoadUser, NULL);
		pthread_join(m_threadLoadMap, NULL);
		pthread_join(m_threadSaveMap, NULL);
		pthread_join(m_threadSaveUser, NULL);

		LOG4CXX_ERROR(logger_, "Threads terminated. Quit.");
		termDatabase();
	}
}

void GameDataSaver::initDatabase()
{
	void * tmp = NULL;
	for(int i = 0; i < db_num; i ++)
	{
		db_user_load.insert(make_pair(i + 1, tmp));
		db_user_save.insert(make_pair(i + 1, tmp));
        db_user_save_backup.insert(make_pair(i + 1, tmp));
		db_user_load_old.insert(make_pair(i + 1, tmp));
		db_plat_load.insert(make_pair(i + 1, tmp));
		db_plat_save.insert(make_pair(i + 1, tmp));
        db_plat_save_backup.insert(make_pair(i + 1, tmp));
		db_plat_load_back.insert(make_pair(i + 1, tmp));
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

        rdb = (TCRDB*)db_user_save_backup[i];
        if (rdb != NULL)
        {
            tcrdbclose(rdb);
            tcrdbdel(rdb);
            db_user_save_backup[i] = NULL;
        }

		rdb = (TCRDB*)db_user_load_old[i];
		if (rdb != NULL)
		{
            tcrdbclose(rdb);
            tcrdbdel(rdb);
            db_user_load_old[i] = NULL;			
		}

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

        rdb = (TCRDB*)db_plat_save_backup[i];
        if (rdb != NULL)
        {
            tcrdbclose(rdb);
            tcrdbdel(rdb);
            db_plat_save_backup[i] = NULL;
        }

		rdb = (TCRDB*)db_plat_load_back[i];
		if (rdb != NULL)
		{
            tcrdbclose(rdb);
            tcrdbdel(rdb);
            db_plat_load_back[i] = NULL;
		}
	}
	db_user_load.clear();
	db_user_save.clear();
    db_user_save_backup.clear();
	db_user_load_old.clear();
	db_plat_load.clear();
	db_plat_save.clear();
    db_plat_save_backup.clear();
	db_plat_load_back.clear();

	if(alloc_rdb != NULL)
	{
		tcrdbclose(alloc_rdb);
		tcrdbdel(alloc_rdb);
		alloc_rdb = NULL;
	}
	if(star_rdb){
		tcrdbclose(star_rdb);
		tcrdbdel(star_rdb);
		star_rdb = NULL;
	}
}

TCRDB* GameDataSaver::getAllocateDb()
{
	TCRDB* rdb = tcrdbnew();
	if (!tcrdbopen(rdb, ServerConfig::Instance().dbUserAddr1(1).c_str(), ServerConfig::Instance().dbUserPort1(1)))
	{
		LOG4CXX_ERROR(logger_, "open db[" << ServerConfig::Instance().dbUserAddr1(1)
			<< ":" << ServerConfig::Instance().dbUserPort1(1)
			<<"] error: " << tcrdberrmsg(tcrdbecode(rdb)));
		tcrdbdel(rdb);
		rdb = NULL;
	}
	
	return rdb;
}

TCRDB* GameDataSaver::getStarDb()
{
	TCRDB* rdb = tcrdbnew();
	if (!tcrdbopen(rdb, ServerConfig::Instance().dbUserAddr1(1).c_str(), ServerConfig::Instance().dbUserPort1(1)))
	{
		LOG4CXX_ERROR(logger_, "open db[" << ServerConfig::Instance().dbUserAddr1(1)
			<< ":" << ServerConfig::Instance().dbUserPort1(1)
			<<"] error: " << tcrdberrmsg(tcrdbecode(rdb)));
		tcrdbdel(rdb);
		rdb = NULL;
	}
	return rdb;
}

int GameDataSaver::getDBId(int64 uid)
{
    return ServerConfig::Instance().dbId(distribution::getRegion(uid));
}

int GameDataSaver::getDBId(const string& platid)
{
    return ServerConfig::Instance().dbId(distribution::getRegion(platid));
}

void GameDataSaver::loadAllocateSetting(int64& uid, int64& iid)
{
	if(alloc_rdb == NULL)
	{
		alloc_rdb = getAllocateDb();
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

void GameDataSaver::saveAllocateSetting(int64& uid, int64& iid)
{
	if(alloc_rdb == NULL)
	{
		alloc_rdb = getAllocateDb();
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

	FILE *fp = fopen(allocate_file, "w");
	if(NULL != fp)
	{
		fprintf(fp, "%lld,%lld", uid, iid);
		fclose(fp);
	}
}

void GameDataSaver::SaveGlobalStarInfo()
{
	DB_GlobalStarInfo starDb;
	StarLogicInst::instance().serizlize(starDb);

	string text;
	starDb.SerializeToString(&text);
	if (!tcrdbput(star_rdb, GlobalStarInfo_DB, strlen(GlobalStarInfo_DB), text.c_str(), text.length()))
	{
		int ecode = tcrdbecode(star_rdb);
		LOG4CXX_ERROR(logger_, "Save DB_GlobalStarInfo ERROR :"<<tcrdberrmsg(ecode));
		tcrdbclose(star_rdb);
		tcrdbdel(star_rdb);
		star_rdb = NULL;
	}
	LOG4CXX_INFO(logger_, "Save DB_GlobalStarInfo SUCCESS ...stars len=" << starDb.stars_size());
}

void GameDataSaver::LoadGlobalStarInfo()
{
	if(star_rdb == NULL)
	{
		star_rdb = getStarDb();
	}
	char* buffer = NULL;
	int len = 0;
	if(star_rdb != NULL)
	{
		buffer = (char*)tcrdbget(star_rdb, GlobalStarInfo_DB, strlen(GlobalStarInfo_DB), &len);
	}
	if (buffer == NULL)
	{
		int ecode = tcrdbecode(star_rdb);
		LOG4CXX_ERROR(logger_, "Get GlobalStarInfoDb ERROR : " << tcrdberrmsg(ecode));
		if (ecode != TTENOREC){
			tcrdbclose(star_rdb);
			tcrdbdel(star_rdb);
			star_rdb = NULL;
		}
		return;
	}

	string data(buffer, len);
	free(buffer);
	DB_GlobalStarInfo StarDb;
	StarDb.ParseFromString(data);
	if(StarLogicInst::instance().unserialize(StarDb)){
		LOG4CXX_INFO(logger_, "Load DB_GlobalStarInfo Success. star size="<<StarDb.stars_size());
	}else{
		LOG4CXX_INFO(logger_, "Load DB_GlobalStarInfo ERROR. star size="<<StarDb.stars_size());
	}
}

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
			struct tm tmnow;
			char szTime[128];
			localtime_r(&now, &tmnow);
			strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", &tmnow);
			fprintf(fp, "%s - %d (%d)\n", szTime, len, count_dirty_);
			fclose(fp);
		}
	}
}

void GameDataSaver::writeSaveError(const char *strError)
{
	time_t now = time(NULL);
	char logfile[256];
	sprintf(logfile,"saveerror%d.log",nid_);
	FILE *fp = fopen(logfile, "a");
	if(NULL != fp)
	{
		struct tm tmnow;
		char szTime[128];
		localtime_r(&now, &tmnow);
		strftime(szTime, 128-1, "%Y-%m-%d %H:%M:%S", &tmnow);
		fprintf(fp, "%s - change db error:%s\n", szTime, strError);
		fclose(fp);
	}
}

void GameDataSaver::writeDisconnectError(const char* strError)
{
    time_t now = time(NULL);
    if (now != log_error_time && now%5 == 0)
    {
        writeSaveError(strError);
        log_error_time = now;
    }
}

int GameDataSaver::safeSaveAllUser(time_t revision, GameDataHandler* const dh, bool force)
{
	// user data
	pthread_mutex_lock(&m_mutexSaveUser);
	map<int64, User*> &users = dh->m_mapDirtyUsers;
	map<int64, User*>::iterator iter = users.begin();
	int list_len = m_listSaveUser.size();
	int save_interval = GameServerConfig::Instance().SaveInterval();
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
		map<int64, User*>::iterator oiter = iter;
		++iter;
		User *user = oiter->second;
		if(force || user->revision() < revision - save_interval * 60 * 1000)
		{
			pushSaveUser(dh, user->uid(), user);
			users.erase(oiter);
		}
	}
	count_dirty_ = users.size();
	pthread_mutex_unlock(&m_mutexSaveUser);
	return 0;
}

int GameDataSaver::safeSaveUser(GameDataHandler* const dh, User* user)
{
	if(user != NULL)
	{
		pthread_mutex_lock(&m_mutexSaveUser);
		pushSaveUser(dh, user->uid(), user);
		pthread_mutex_unlock(&m_mutexSaveUser);
		LOG4CXX_DEBUG(logger_, "Save user data:uid=" << user->uid());
	}
	return 0;
}

int GameDataSaver::safeLoadUser(GameDataHandler* const dh, int64& uid)
{
	pthread_mutex_lock(&m_mutexLoadUser);
	pushLoadUser(dh, uid);
	pthread_mutex_unlock(&m_mutexLoadUser);
	LOG4CXX_DEBUG(logger_, "Load user data " << uid);
	return 0;
}

int GameDataSaver::safeSaveAllMap(GameDataHandler* const dh)
{
	// map data
	pthread_mutex_lock(&m_mutexSaveMap);
	hash_map<string, int64> &list = dh->m_mapPlatId2Uid;
	for (hash_map<string, int64>::iterator iter = list.begin(); iter!=list.end(); ++iter)
	{
		const string& pid = iter->first;
		int64 uid = iter->second;
		if (uid > 0)
		{
			pushSaveMap(dh, pid, uid);
		}
	}
	pthread_mutex_unlock(&m_mutexSaveMap);
	LOG4CXX_DEBUG(logger_, "All PlatformID->UID Map saved.");
	return 0;
}

int GameDataSaver::safeSaveMap(GameDataHandler* const dh, const string& pid, int64& uid)
{
	if(uid > 0)
	{
		pthread_mutex_lock(&m_mutexSaveMap);
		pushSaveMap(dh, pid, uid);
		pthread_mutex_unlock(&m_mutexSaveMap);
		LOG4CXX_DEBUG(logger_, "Save platid map: platid="<<pid<<"->uid="<<uid);
	}
	return 0;
}

int GameDataSaver::safeLoadMap(GameDataHandler* const dh, const string& pid)
{
	pthread_mutex_lock(&m_mutexLoadMap);
	pushLoadMap(dh, pid);
	pthread_mutex_unlock(&m_mutexLoadMap);
	LOG4CXX_DEBUG(logger_, "Load user map " << pid);
	return 0;
}

int GameDataSaver::pushSaveUser(GameDataHandler* const dh, int64 uid, User* user)
{
	string * data = new string;
	MakeUserString(dh, user, *data);

	list<UserItem>::iterator iter = m_listSaveUser.begin();
	while(iter != m_listSaveUser.end())
	{
		if(iter->uid == uid)
		{
			delete iter->data;
			iter->data = data;
			return 0;
		}
		++iter;
	}

	UserItem save_data;
	save_data.dh = dh;
	save_data.data = data;
	save_data.uid = uid;

	m_listSaveUser.push_front(save_data);
	//user->setRevision(dh->revision());
	return 0;
}

int GameDataSaver::pushLoadUser(GameDataHandler* const dh, int64& uid)
{
	UserItem load_data;
	load_data.dh = dh;
	load_data.data = NULL;
	load_data.uid = uid;

	m_listLoadUser.push_front(load_data);
	return 0;
}

int GameDataSaver::pushSaveMap(GameDataHandler* const dh, const string& platid,
							   int64& uid)
{
	MapItem save_data;
	save_data.dh = dh;
	save_data.platid = platid;
	save_data.uid = uid;
	
	m_listSaveMap.push_front(save_data);
	return 0;
}

int GameDataSaver::pushLoadMap(GameDataHandler* const dh, const string& platid)
{
	MapItem load_data;
	load_data.dh = dh;
	load_data.platid = platid;
	load_data.uid = 0;

	m_listLoadMap.push_front(load_data);
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

TCRDB* GameDataSaver::getUserLoadDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_user_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbUserAddr1(dbid), ServerConfig::Instance().dbUserPort1(dbid));
	}

	if(rdb != NULL)
	{
		db_user_load[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getUserLoadOldDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_user_load_old[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbUserAddrOld(dbid), ServerConfig::Instance().dbUserPortOld(dbid));
	}

	if(rdb != NULL)
	{
		db_user_load_old[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getUserLoadBackDb( int dbid )
{
	TCRDB *rdb = (TCRDB *)db_user_load_back[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbUserBack(dbid), ServerConfig::Instance().dbUserPortback(dbid));
	}

	if(rdb != NULL)
	{
		db_user_load_back[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getUserSaveDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_user_save[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbUserAddr1(dbid), ServerConfig::Instance().dbUserPort1(dbid));
	}

	if(rdb != NULL)
	{
		db_user_save[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getUserSaveDbBackup(int dbid)
{
    TCRDB *rdb = (TCRDB *)db_user_save_backup[dbid];
    if(rdb == NULL)
    {
        rdb = getDb(ServerConfig::Instance().dbUserAddr2(dbid), ServerConfig::Instance().dbUserPort2(dbid));
    }

    if(rdb != NULL)
    {
        db_user_save_backup[dbid] = rdb;
    }
    return rdb;
}

TCRDB* GameDataSaver::getPlatLoadDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_plat_load[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbPlatAddr1(dbid), ServerConfig::Instance().dbPlatPort1(dbid));
	}

	if(rdb != NULL)
	{
		db_plat_load[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getPlatLoadBackDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_plat_load_back[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbPlatBack(dbid), ServerConfig::Instance().dbPlatPortback(dbid));
	}

	if(rdb != NULL)
	{
		db_plat_load_back[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getPlatSaveDb(int dbid)
{
	TCRDB *rdb = (TCRDB *)db_plat_save[dbid];
	if(rdb == NULL)
	{
		rdb = getDb(ServerConfig::Instance().dbPlatAddr1(dbid), ServerConfig::Instance().dbPlatPort1(dbid));
	}

	if(rdb != NULL)
	{
		db_plat_save[dbid] = rdb;
	}
	return rdb;
}

TCRDB* GameDataSaver::getPlatSaveDbBackup(int dbid)
{
    TCRDB *rdb = (TCRDB *)db_plat_save_backup[dbid];

    if(rdb == NULL)
    {
        rdb = getDb(ServerConfig::Instance().dbPlatAddr2(dbid), ServerConfig::Instance().dbPlatPort2(dbid));
    }

    if(rdb != NULL)
    {
        db_plat_save_backup[dbid] = rdb;
    }
    return rdb;
}

int GameDataSaver::saveUser(GameDataHandler* const dh, int64& uid, string& data)
{
	int dbid = getDBId(uid);
	TCRDB* rdb = getUserSaveDb(dbid);
	if(rdb == NULL)
	{
        writeDisconnectError("Put User Error : Disconnect");
		return -1;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	const char* buf = data.c_str();
	int len = data.length();
	if(!tcrdbput(rdb, suid, 16, buf, len))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put user ERROR : " << tcrdberrmsg(ecode));
		writeSaveError(tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_user_save[dbid] = NULL;
		return -1;
	}
	LOG4CXX_DEBUG(logger_, "Saved User: uid="<<uid);
	return 0;
}

int GameDataSaver::saveUserBackup(GameDataHandler* const dh, int64& uid, string& data)
{
    if (GameServerConfig::Instance().BackupDBEnable() == 0)
    {
        return 0;
    }

    int dbid = getDBId(uid);
    TCRDB* rdb = getUserSaveDbBackup(dbid);
    if(rdb == NULL)
    {
        writeDisconnectError("Put User backup Error : Disconnect");
        return -1;
    }

    char suid[32];
    sprintf(suid, "%016llX", uid);
    const char* buf = data.c_str();
    int len = data.length();
    if(!tcrdbput(rdb, suid, 16, buf, len))
    {
        int ecode = tcrdbecode(rdb);
        LOG4CXX_ERROR(logger_, "Put user backup ERROR : " << tcrdberrmsg(ecode));
        writeSaveError(tcrdberrmsg(ecode));
        tcrdbclose(rdb);
        tcrdbdel(rdb);
        db_user_save_backup[dbid] = NULL;
        return -1;
    }
    LOG4CXX_DEBUG(logger_, "Saved User backup: uid="<<uid);
    return 0;
}

int GameDataSaver::loadUser(GameDataHandler* const dh, int64& uid)
{
	int dbid = getDBId(uid);
	TCRDB* rdb = getUserLoadDb(dbid);
	if (rdb == NULL)
	{
        writeDisconnectError("Load User Error : Disconnect");
		return -1;
	}
	char suid[32];
	sprintf(suid, "%016llX", uid);

	User* user = NULL;
	int len;
	char* buffer = (char*)tcrdbget(rdb, suid, 16, &len);
	if (buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if (ecode == TTENOREC) 
		{
			if (ServerConfig::Instance().GetServerChannel() != CHANNEL_UC)
	        {
	        	dh->updateLoadUser(uid, user);
	            return -1;
	        }
			rdb = getUserLoadOldDb(dbid);
			if (rdb == NULL)
			{
				writeDisconnectError("Load User Old Error : Disconnect");
				return -1;
			}
			buffer = (char*)tcrdbget(rdb, suid, 16, &len);
			if (buffer == NULL)
			{
				ecode = tcrdbecode(rdb);
				if (ecode == TTENOREC)
				{
					rdb = getUserLoadBackDb(dbid);
					if (rdb == NULL)
					{
						writeDisconnectError("Load User Back Error : Disconnect");
						return -1;
					}
					buffer = (char*)tcrdbget(rdb, suid, 16, &len);
					if (buffer == NULL)
					{
						ecode = tcrdbecode(rdb);
						if (ecode == TTENOREC)
						{
							dh->updateLoadUser(uid, user);
							return 1;
						}
						LOG4CXX_ERROR(logger_, "Get user Back ERROR : " << tcrdberrmsg(ecode));
						tcrdbclose(rdb);
						tcrdbdel(rdb);
						db_user_load_back[dbid] = NULL;
						return -1;
					}
				}
				else
				{
					LOG4CXX_ERROR(logger_, "Get user Old ERROR : " << tcrdberrmsg(ecode));
					tcrdbclose(rdb);
					tcrdbdel(rdb);
					db_user_load_old[dbid] = NULL;
					return -1;
				}
			}
		}
		else
		{
			LOG4CXX_ERROR(logger_, "Get user ERROR : " << tcrdberrmsg(ecode));
			tcrdbclose(rdb);
			tcrdbdel(rdb);
			db_user_load[dbid] = NULL;
			return -1;
		}
	}
	LOG4CXX_DEBUG(logger_, "Loaded User: uid="<<uid <<" : Length="<<len);

	string data(buffer, len);
	dh->acquireDataLock();
	user = new User();
	LoadUserFromString(dh, user, data);
	dh->releaseDataLock();
	dh->updateLoadUser(uid, user);
	free(buffer);
	return 0;
}

int GameDataSaver::saveMap(GameDataHandler* const dh, const string& pid, int64& uid)
{
	int dbid = getDBId(pid);
	TCRDB* rdb = getPlatSaveDb(dbid);
	if(rdb == NULL)
	{
        writeDisconnectError("Put map Error : Disconnect");
		return -1;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	if(!tcrdbput2(rdb, pid.c_str(), suid))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "Put map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_plat_save[dbid] = NULL;
		return -1;
	}
	LOG4CXX_DEBUG(logger_, "Saved Map:platid="<<pid<<"->uid="<<uid);
	return 0;
}

int GameDataSaver::saveMapBackup(GameDataHandler* const dh, const string&pid, int64& uid)
{
    if (GameServerConfig::Instance().BackupDBEnable() == 0)
    {
        return 0;
    }

    int dbid = getDBId(pid);
    TCRDB* rdb = getPlatSaveDbBackup(dbid);
    if(rdb == NULL)
    {
        writeDisconnectError("Put map backup Error : Disconnect");
        return -1;
    }

    char suid[32];
    sprintf(suid, "%016llX", uid);
    if(!tcrdbput2(rdb, pid.c_str(), suid))
    {
        int ecode = tcrdbecode(rdb);
        LOG4CXX_ERROR(logger_, "Put map backup ERROR : " << tcrdberrmsg(ecode));
        tcrdbclose(rdb);
        tcrdbdel(rdb);
        db_plat_save_backup[dbid] = NULL;
        return -1;
    }
    LOG4CXX_DEBUG(logger_, "Saved Map backup:platid="<<pid<<"->uid="<<uid);
    return 0;
}

int GameDataSaver::loadMap(GameDataHandler* const dh, const string& platid)
{
	int dbid = getDBId(platid);
	TCRDB* rdb = getPlatLoadDb(dbid);
	if(rdb == NULL)
	{
        writeDisconnectError("Load map Error : Disconnect");
		return -1;
	}

	int64 uid = 0;
	char* buffer = (char*)tcrdbget2(rdb, platid.c_str());
	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode == TTENOREC) {
			dh->updateLoadPlatid(platid, uid);
			return 1;
		}
		LOG4CXX_ERROR(logger_, "Get map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_plat_load[dbid] = NULL;
		return -1;
	}

	sscanf(buffer, "%016llX", &uid);
	dh->updateLoadPlatid(platid, uid);
	LOG4CXX_DEBUG(logger_, "Loaded Map:platid="<<platid<<"->uid="<<uid);
	free(buffer);
	return 0;
}

int GameDataSaver::GMloadMapFromBack(GameDataHandler* const dh, const string& platid)
{
	int dbid = getDBId(platid);
	TCRDB* rdb = getPlatLoadBackDb(dbid);
	if(rdb == NULL)
	{
        writeDisconnectError("Load map Error : Disconnect");
		return -1;
	}

	int64 uid = 0;
	char* buffer = (char*)tcrdbget2(rdb, platid.c_str());
	if(buffer == NULL)
	{
		int ecode = tcrdbecode(rdb);
		if(ecode == TTENOREC) {
			dh->updateLoadPlatid(platid, uid);
			return 1;
		}
		LOG4CXX_ERROR(logger_, "Get map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_plat_load[dbid] = NULL;
		return -1;
	}

	sscanf(buffer, "%016llX", &uid);
	dh->updateLoadPlatid(platid, uid);
	dh->savePlatidMap(platid, uid);
	LOG4CXX_DEBUG(logger_, "Loaded Map:platid="<<platid<<"->uid="<<uid);
	free(buffer);
	return 0;
}

void GameDataSaver::RemovePlatidFromDb(const string& pid)
{
	int dbid = getDBId(pid);
	TCRDB* rdb = getPlatLoadDb(dbid);
	if(rdb == NULL)
	{
		return;
	}

	if(!tcrdbout(rdb, pid.c_str(), pid.length()))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "out map ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		db_plat_save[dbid] = NULL;
		return;
	}
	LOG4CXX_DEBUG(logger_, "out Map:platid="<<pid);
}

void GameDataSaver::RemoveUserFromDb(int64 uid)
{
	int dbid = getDBId(uid);
	TCRDB* rdb = getUserLoadDb(dbid);
	if(rdb == NULL)
	{
		return;
	}

	char suid[32];
	sprintf(suid, "%016llX", uid);
	if(!tcrdbout(rdb, suid, 16))
	{
		int ecode = tcrdbecode(rdb);
		LOG4CXX_ERROR(logger_, "out user ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(rdb);
		tcrdbdel(rdb);
		return;
	}
	LOG4CXX_DEBUG(logger_, "out user->uid="<<uid);
}

void GameDataSaver::routineUser(ActionType type)
{
	int ret = 0, ret_backup = 0;
	pthread_mutex_t* mutex_ = (type == ACTION_USER_LOAD)?&m_mutexLoadUser:&m_mutexSaveUser;
	list<UserItem>* list = (type == ACTION_USER_LOAD)? &m_listLoadUser : &m_listSaveUser;

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
				usleep(5000);
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
			ret = saveUser(item.dh, item.uid, *item.data);
            ret_backup = saveUserBackup(item.dh, item.uid, *item.data);
		}
		else
		{
			ret = loadUser(item.dh, item.uid);
            ret_backup = 0;
		}
		if(ret >= 0 && ret_backup >= 0)
		{
			if(item.data != NULL)
			{
				delete item.data;
				item.data = NULL;
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
	}
}

void GameDataSaver::routineMap(ActionType type)
{
	int ret = 0, ret_backup = 0;
	pthread_mutex_t* mutex_ = (type == ACTION_MAP_LOAD)?&m_mutexLoadMap:&m_mutexSaveMap;
	list<MapItem>* list = (type == ACTION_MAP_LOAD)? &m_listLoadMap : &m_listSaveMap;

	while(true)
	{
		pthread_mutex_lock(mutex_);
		if(list->empty())
		{
			pthread_mutex_unlock(mutex_);
			if(b_serving)
			{
				usleep(5000);
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
			ret = saveMap(item.dh, item.platid, item.uid);
            ret_backup = saveMapBackup(item.dh, item.platid, item.uid);
		}
		else
		{
			ret = loadMap(item.dh, item.platid);
            ret_backup = 0;
		}
		if(ret < 0 || ret_backup < 0)
		{
			// retry
			pthread_mutex_lock(mutex_);
			list->push_front(item);
			pthread_mutex_unlock(mutex_);
			usleep(500);
			continue;
		}
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

void* GameDataSaver::saveMapThread(void* arg)
{
	GameDataSaver* ds = static_cast<GameDataSaver*>(arg);
	ds->routineMap(ACTION_MAP_SAVE);
	pthread_exit(NULL);
	return NULL;
}

void* GameDataSaver::loadMapThread(void* arg)
{
	GameDataSaver* ds = static_cast<GameDataSaver*>(arg);
	ds->routineMap(ACTION_MAP_LOAD);
	pthread_exit(NULL);
	return NULL;
}

void GameDataSaver::MakeUserString(GameDataHandler *dh, User *pUser, string &oString )
{
	if (pUser == NULL)
	{
		return;
	}
	DB_User& dbUser = pUser->GetDbUser();
	dbUser.SerializeToString( &oString );
}

void GameDataSaver::LoadUserFromString(GameDataHandler *dh, User *pUser, string &oString )
{
	if (pUser == NULL)
	{
		return;
	}
	DB_User dbUser;
	dbUser.ParseFromString( oString );
	pUser->SetDbUser(dbUser);
}
