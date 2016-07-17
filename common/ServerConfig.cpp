#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "ServerConfig.h"

ServerConfig ServerConfig::m_ServerConfig;
const char* ServerConfig::szConfigFile = "server.cfg";
const char SZSYSLOGCFG[] = "syslog.cfg";

ServerConfig::ServerConfig()
{
	if (!loadFromFile()) {
		printf("WARNING! read config file failed.\n");
		exit(1);
	}
}
ServerConfig::~ServerConfig(void)
{
}

#ifdef _WIN32

const char* ServerConfig::szConfigFile_Win32 = "server.win32.cfg";
void ServerConfig::copyFileForWindows()
{
    CopyFile(szConfigFile, szConfigFile_Win32, true);
}

#endif


bool ServerConfig::loadFromFile()
{

#ifdef _WIN32
    copyFileForWindows();
#endif

	//server.cfg
	try
	{

#ifdef _WIN32
        srv.readFile(szConfigFile_Win32);
#else
        srv.readFile(szConfigFile);
#endif

        string strLocal = srv.lookup("app.locale");
        if (strLocal == "ZH_CN") locale_ = LOCALE_ZH_CN;
        else if(strLocal == "ZH_TW") locale_ = LOCALE_ZH_TW;
        else if(strLocal == "EN_US") locale_ = LOCALE_EN_US;
        else if(strLocal == "TR_TR") locale_ = LOCALE_TR_TR;
		else if(strLocal == "EN_EN") locale_ = LOCALE_EN_EN;
		else if(strLocal == "JP_JP") locale_ = LOCALE_JP_JP;
		else if(strLocal == "JP_JP2") locale_ = LOCALE_JP_JP2;
		else if(strLocal == "RUS_RUS") locale_ = LOCALE_RUSSIA;
		else if(strLocal == "GER_GER")	locale_ = LOCALE_GERMANY;
		else if(strLocal == "FRA_FRA") locale_ = LOCALE_FRANCE;
		else if(strLocal == "TUR_TUR") locale_ = LOCALE_TUR_TUR;
		else if(strLocal == "POR_POR") locale_ = LOCALE_POR_POR;
		else if(strLocal == "ITA_ITA") locale_ = LOCAL_ITA_ITA;
		else if(strLocal == "MID_EAST") locale_ = LOCAL_MID_EAST;
		else if(strLocal == "MID_EAST_ARAB") locale_ = LOCAL_MID_EAST_ARAB;
        else locale_ = LOCALE_ZH_CN;

        string strChannel = srv.lookup("app.channel");
        if (strChannel == "APPSTORE") channel_ = CHANNEL_APPSTORE;
		else if (strChannel == "YYB") channel_ = CHANNEL_YYB;
		else if (strChannel == "UC") channel_ = CHANNEL_UC;
		else if (strChannel == "GPLAY") channel_ = CHANNEL_GPLAY;
        else channel_ = CHANNEL_APPSTORE;

        test_ = srv.lookup("app.testServer");

		// worldd
		world_id_.clear();
		world_web_server_port_.clear();
		world_server_address_.clear();
		world_gamed_port_.clear();
		world_bind_ip_.clear();
		worldd_localdb_port_.clear();
		worldd_num_ = srv.lookup("app.worldd_num");
		Setting &worldd = srv.lookup("app.worldd");
		for( int i =0; i< worldd_num_; i++ )
		{
			int id = worldd[i]["id"];
			world_id_.push_back(id);
			world_web_server_port_.insert(make_pair(id, (int)worldd[i]["world_web_server_port"]));
			world_server_address_.insert(make_pair(id, (const char*)worldd[i]["world_server_address"]));
			world_gamed_port_.insert(make_pair(id,(int) worldd[i]["world_gamed_port"]));
			world_bind_ip_.insert(make_pair(id, (const char*)worldd[i]["world_bind_ip"]));
			if (worldd[i].exists("localdb_port"))
				worldd_localdb_port_.insert(make_pair(id, (int)worldd[i]["localdb_port"]));			
		}

		
		// proxy
		proxy_addr.clear();
		proxy_port_1.clear();
		proxy_port_2.clear();
		proxy_policy_port.clear();
		proxy_num_ = srv.lookup("app.proxy_num");
		Setting &proxyd = srv.lookup("app.proxy");
		for(int i =0; i< proxy_num_; i++)
		{
			int id = proxyd[i]["id"];
			proxy_addr.insert(make_pair(id, (const char*)proxyd[i]["proxy_address"]));
			proxy_port_1.insert(make_pair(id, (int)proxyd[i]["proxy_port_1"]));
			proxy_port_2.insert(make_pair(id,(int) proxyd[i]["proxy_port_2"]));
			proxy_policy_port.insert(make_pair(id, (int)proxyd[i]["proxy_policy_port"]));
		}

		// gamed
		gamed_ip_.clear();
		gamed_port_.clear();
		gamed_bind_ip_.clear();
		gamed_server_port_.clear();
		gamed_policy_port_.clear();
        gamed_localdb_port_.clear();
		gamed_num_ = srv.lookup("app.gamed_num");
        
		printf("gamed_num:%d\n", gamed_num_);
		Setting &gamed = srv.lookup("app.gamed");

		for (int i=0; i<gamed_num_; i++) {
			int gamed_id = gamed[i]["id"];
			gamed_ip_.insert(make_pair(gamed_id, (const char *)gamed[i]["addr"]));
			gamed_port_.insert(make_pair(gamed_id, (int) gamed[i]["port"]));
			gamed_bind_ip_.insert(make_pair(gamed_id, (const char*) gamed[i]["bind_addr"]));
			gamed_server_port_.insert(make_pair(gamed_id, (int)gamed[i]["server_port"]));
			gamed_policy_port_.insert(make_pair(gamed_id, (int)gamed[i]["policy_port"]));
            gamed_localdb_port_.insert(make_pair(gamed_id, (int)gamed[i]["localdb_port"]));
			
		}

		// database
		db_user_addr_1.clear();
		db_user_port_1.clear();
		db_user_addr_2.clear();
		db_user_port_2.clear();
		db_plat_addr_1.clear();
		db_plat_port_1.clear();
		db_plat_addr_2.clear();
		db_plat_port_2.clear();
		db_user_addr_old.clear();
		db_user_port_old.clear();
		db_user_addr_back.clear();
		db_user_port_back.clear();
		db_plat_addr_back.clear();
		db_plat_port_back.clear();
		db_num_ = srv.lookup("app.db_num");
		printf("db_num:%d\n", db_num_);
		Setting &db = srv.lookup("app.database");

		for (int i=0; i<db_num_; i++) {
			int db_id = db[i]["id"];
			db_user_addr_1.insert(make_pair(db_id, (const char*)db[i]["addr_user_1"]));
			db_user_port_1.insert(make_pair(db_id, (int)db[i]["port_user_1"]));
			db_plat_addr_1.insert(make_pair(db_id, (const char*)db[i]["addr_plat_1"]));
			db_plat_port_1.insert(make_pair(db_id, (int)db[i]["port_plat_1"]));
			db_user_addr_2.insert(make_pair(db_id, (const char*)db[i]["addr_user_2"]));
			db_user_port_2.insert(make_pair(db_id, (int)db[i]["port_user_2"]));
			db_plat_addr_2.insert(make_pair(db_id, (const char*)db[i]["addr_plat_2"]));
			db_plat_port_2.insert(make_pair(db_id, (int)db[i]["port_plat_2"]));
			if (db[i].exists("addr_user_old"))
				db_user_addr_old.insert(make_pair(db_id, (const char*)db[i]["addr_user_old"]));
			if (db[i].exists("port_user_old"))
				db_user_port_old.insert(make_pair(db_id, (int)db[i]["port_user_old"]));
			if (db[i].exists("addr_user_back"))
				db_user_addr_back.insert(make_pair(db_id, (const char*)db[i]["addr_user_back"]));
			if (db[i].exists("port_user_back"))
				db_user_port_back.insert(make_pair(db_id, (int)db[i]["port_user_back"]));
			if (db[i].exists("addr_plat_back"))
				db_plat_addr_back.insert(make_pair(db_id, (const char*)db[i]["addr_plat_back"]));
			if (db[i].exists("port_plat_back"))
				db_plat_port_back.insert(make_pair(db_id, (int)db[i]["port_plat_back"]));
		}

        //site
        region_newregionid.clear();
        region_dbid.clear();
        region_gameid.clear();
        game_newregionid.clear();
        region_num_ = srv.lookup("app.region_num");
        printf("region_num:%d\n", region_num_);
        Setting &region_setting = srv.lookup("app.region");
        for (int i = 0; i < region_num_; i++)
        {
            int regionid = region_setting[i]["id"];
            region_newregionid.insert(make_pair(regionid, (int)region_setting[i]["new_region"]));
            region_dbid.insert(make_pair(regionid, (int)region_setting[i]["dbid"]));
            region_gameid.insert(make_pair(regionid, (int)region_setting[i]["gameid"]));
            game_newregionid.insert(make_pair((int)region_setting[i]["gameid"], (int)region_setting[i]["new_region"]));
        }
	}
	catch (ParseException ex)
	{
		printf("Parsing config file %s failed.\n", szConfigFile);
		return false;
	}
	catch (FileIOException ex) {
		printf("Read config file %s failed. IOExcetpion.\n", szConfigFile);
		return false;
	}
	catch (SettingNotFoundException ex) {
		printf("Read config file %s failed. Setting \"%s\" not found.\n", szConfigFile, ex.getPath());
		return false;
	}

	//syslog.cfg
	try
	{
		srv.readFile(SZSYSLOGCFG);

		Setting &gamed = srv.lookup("app.gamed");
		{
			gamed_log_start_			= (int)gamed["log_file_start"];
			gamed_log_dir_				= (const char *)gamed["log_file_dir"];
			gamed_log_name_				= (const char *)gamed["log_file_name"];
			gamed_log_stat_level_		= (int)gamed["send_lv"];
			gamed_log_stat_modul_		= (int)gamed["user_mod"];
			gamed_log_stat_modul_val_	= (int)gamed["user_mod_val"];
			gamed_log_stat_addr_		= (const char *)gamed["statsrv_addr"];
			gamed_log_stat_port_		= (const char *)gamed["statsrv_port"];
		}
		Setting &worldd = srv.lookup("app.worldd");
		{
			worldd_log_start_			= (int)worldd["log_file_start"];
			worldd_log_dir_				= (const char *)worldd["log_file_dir"];
			worldd_log_name_			= (const char *)worldd["log_file_name"];
			worldd_log_stat_level_		= (int)worldd["send_lv"];
			worldd_log_stat_modul_		= (int)worldd["user_mod"];
			worldd_log_stat_modul_val_	= (int)worldd["user_mod_val"];
			worldd_log_stat_addr_		= (const char *)worldd["statsrv_addr"];
			worldd_log_stat_port_		= (const char *)worldd["statsrv_port"];
		}
	}

	catch (ParseException ex)
	{
		//printf("Parsing config file %s failed.%d\n", SZSYSLOGCFG, ex.getLine());
		printf("Parsing config file %s failed.", SZSYSLOGCFG);
		return false;
	}
	catch (FileIOException ex) {

		printf("Parsing config file %s failed.\n", SZSYSLOGCFG);
		//return false;
	}
	catch (SettingNotFoundException ex) {
		printf("Parsing config file %s failed. Setting \"%s\" not found.\n", SZSYSLOGCFG, ex.getPath());
		return false;
	}

	return true;
}

int ServerConfig::worlddGamedPort(int id) 
{
	map<int, int>::const_iterator iter = world_gamed_port_.find(id);
	if (iter==world_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

int ServerConfig::worlddWebPort(int id)
{
	map<int, int>::const_iterator iter = world_web_server_port_.find(id);
	if (iter==world_web_server_port_.end()) 
		return 0;
	return iter->second; 
}

const string ServerConfig::worlddIp(int id) 
{
	map<int, string>::const_iterator iter = world_server_address_.find(id);
	if (iter==world_server_address_.end()) 
		return "";
	return iter->second; 
}

const string ServerConfig::worlddBindIp(int id)
{
	map<int, string>::const_iterator iter = world_bind_ip_.find(id);
	if (iter==world_bind_ip_.end()) 
		return "";
	return iter->second; 
}

const string ServerConfig::gamedIp(int id) {
	map<int, string>::const_iterator iter = gamed_ip_.find(id);
	if (iter==gamed_ip_.end())
		return "";
	return iter->second;
}

int ServerConfig::gamedPort(int id) {
	map<int, int>::const_iterator iter = gamed_port_.find(id);
	if (iter==gamed_port_.end())
		return 0;
	return iter->second;
}

const string ServerConfig::gamedBindIp(int id) {
	map<int, string>::const_iterator iter = gamed_bind_ip_.find(id);
	if (iter==gamed_bind_ip_.end())
		return "";
	return iter->second;
}

