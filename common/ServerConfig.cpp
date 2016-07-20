#include <time.h>
#include "ServerConfig.h"
#include "HotCacheDBCfg.h"

ServerConfig::ServerConfig(const char* server_file): server_file_(server_file)
{
	if (!loadFromFile()) {
		printf("WARNING! read config file failed.\n");
		exit(1);
	}
}

ServerConfig::~ServerConfig(void)
{
}

bool ServerConfig::loadFromFile()
{
	if(!HotCacheDBCfg::Instance().LoadTbl())
	{
		printf("WARNING! read HotCacheDb.cfg file failed.\n");
		return false;
	}
	//server.cfg
	try
	{
		srv.readFile(server_file_.c_str());

		//分区信息
		region_id_.clear();
		m_mapGameRegion.clear();
		m_mapRegionToGroup.clear();
		m_mapGroupRegion.clear();
		m_mapRegionResetFlag.clear();

		physics_game_num_		= srv.lookup("app.physics_game_num");
		int physics_region_num	= srv.lookup("app.physics_region_num"); 

		region_num_ = srv.lookup("app.logic_region_num");
		Setting &region = srv.lookup("app.region");
		for (int i=0;i<region_num_;i++)
		{
			int id			= region[i]["id"];
			int groupid		= region[i]["groupid"];
			int gameregion	= region[i]["physics_region"];
			//int nFlag		= region[i]["reset_flag"];
			region_id_.push_back(id);
			if(groupid>=0)
			{
				m_mapRegionToGroup[id] = groupid;
				m_mapGroupRegion[groupid].push_back(id);
				if(groupid == id)
				{
					m_mapRegionResetFlag[groupid] = region[i]["reset_flag"];
				}
			}
			if(gameregion<physics_game_num_)
			{
				m_mapGameRegion[id] = gameregion;
			}
			else
			{
				printf("Read ServerConfig.cfg File ERROR:physics_game_num read error!!!! \r\n");
				return false;
			}
		}

		// worldd
		world_id_.clear();
		world_web_server_port_.clear();
		world_server_address_.clear();
		world_gamed_port_.clear();
		world_bind_ip_.clear();
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

		// star srv
		star_id_.clear();
		star_server_address_.clear();
		star_gamed_port_.clear();
		star_bind_ip_.clear();
		star_num_ = srv.lookup("app.starsrv_num");
		Setting &pksrv = srv.lookup("app.star");
		for ( int i = 0; i < star_num_; i++ )
		{
			int id = pksrv[i]["id"];
			star_id_.push_back( id );
			star_server_address_.insert( make_pair( id, (const char*) pksrv[i]["addr"]));
			star_gamed_port_.insert( make_pair( id, (int) pksrv[i]["port"]));
			star_bind_ip_.insert( make_pair( id, (const char*) pksrv[i]["bind_addr"]));
		}
		//country srv
		country_id_.clear();
		country_server_address_.clear();
		country_gamed_port_.clear();
		country_bind_ip_.clear();
		country_num_ = srv.lookup("app.countrysrv_num");
		Setting &countrysrv = srv.lookup("app.country");
		for ( int i = 0; i < country_num_; i++ )
		{
			int id = countrysrv[i]["id"];
			country_id_.push_back( id );
			country_server_address_.insert( make_pair( id, (const char*) countrysrv[i]["addr"]));
			country_gamed_port_.insert( make_pair( id, (int) countrysrv[i]["port"]));
			country_bind_ip_.insert( make_pair( id, (const char*) countrysrv[i]["bind_addr"]));
		}


		//region srv
		regionsrv_id_.clear();
		region_server_address_.clear();
		region_gamed_port_.clear();
		regionsrv_bind_ip_.clear();
		regionsrv_num_ = srv.lookup("app.regionsrv_num");
		Setting &regionsrv = srv.lookup("app.regionsrv");
		for ( int i = 0; i < regionsrv_num_; i++ )
		{
			int id = regionsrv[i]["id"];
			regionsrv_id_.push_back( id );
			region_server_address_.insert( make_pair( id, (const char*) regionsrv[i]["addr"]));
			region_gamed_port_.insert( make_pair( id, (int) regionsrv[i]["port"]));
			regionsrv_bind_ip_.insert( make_pair( id, (const char*) regionsrv[i]["bind_addr"]));
		}
		//plat srv
		plat_id_.clear();
		plat_server_address_.clear();
		plat_gamed_port_.clear();
		plat_bind_ip_.clear();
		plat_num_ = srv.lookup("app.platsrv_num");
		Setting &platsrv = srv.lookup("app.plat");
		for ( int i = 0; i < plat_num_; i++ )
		{
			int id = platsrv[i]["id"];
			plat_id_.push_back( id );
			plat_server_address_.insert( make_pair( id, (const char*) platsrv[i]["addr"]));
			plat_gamed_port_.insert( make_pair( id, (int) platsrv[i]["port"]));
			plat_bind_ip_.insert( make_pair( id, (const char*) platsrv[i]["bind_addr"]));
		}

		//fight srv
		fight_id_.clear();
		fight_server_address_.clear();
		fight_gamed_port_.clear();
		fight_bind_ip_.clear();
		fight_num_ = srv.lookup("app.fightd_num");
		Setting &fightsrv = srv.lookup("app.fightd");
		for ( int i = 0; i < fight_num_; i++ )
		{
			int id = fightsrv[i]["id"];
			fight_id_.push_back( id );
			fight_server_address_.insert( make_pair( id, (const char*) fightsrv[i]["addr"]));
			fight_gamed_port_.insert( make_pair( id, (int) fightsrv[i]["port"]));
			fight_bind_ip_.insert( make_pair( id, (const char*) fightsrv[i]["bind_addr"]));
		}


		//rank srv
		ranksrv_id_.clear();
		rank_server_address_.clear();
		rank_gamed_port_.clear();
		ranksrv_bind_ip_.clear();
		ranksrv_num_ = srv.lookup("app.ranksrv_num");
		Setting &ranksrv = srv.lookup("app.ranksrv");
		for ( int i = 0; i < ranksrv_num_; i++ )
		{
			int id = ranksrv[i]["id"];
			ranksrv_id_.push_back( id );
			rank_server_address_.insert( make_pair( id, (const char*) ranksrv[i]["addr"]));
			rank_gamed_port_.insert( make_pair( id, (int) ranksrv[i]["port"]));
			ranksrv_bind_ip_.insert( make_pair( id, (const char*) ranksrv[i]["bind_addr"]));
		}


		//stat srv
		//statsrv_id_.clear();
		//stat_server_address_.clear();
		//stat_gamed_port_.clear();
		//statsrv_bind_ip_.clear();
		//statsrv_num_ = srv.lookup("app.statsrv_num");
		//Setting &statsrv = srv.lookup("app.statsrv");
		//for ( int i = 0; i < statsrv_num_; i++ )
		//{
		//	int id = statsrv[i]["id"];
		//	statsrv_id_.push_back( id );
		//	stat_server_address_.insert( make_pair( id, (const char*) statsrv[i]["addr"]));
		//	stat_gamed_port_.insert( make_pair( id, (int) statsrv[i]["port"]));
		//	statsrv_bind_ip_.insert( make_pair( id, (const char*) statsrv[i]["bind_addr"]));
		//}

		//web srv
		web_id_.clear();
		web_server_address_.clear();
		web_gamed_port_.clear();
		web_num_ = srv.lookup("app.web_num");
		Setting &websrv = srv.lookup("app.webserver");
		for ( int i = 0; i < web_num_; i++ )
		{
			int id = websrv[i]["id"];
			web_id_.push_back( id );
			web_server_address_.insert( make_pair( id, (const char*) websrv[i]["addr"]));
			web_gamed_port_.insert( make_pair( id, (int) websrv[i]["port"]));
		}

		// transfer srv
		transfer_id_.clear();
		transfer_server_address_.clear();
		transfer_gamed_port_.clear();
		transfer_bind_ip_.clear();
		transfer_num_ = 0;
		//transfer_num_ = srv.lookup("app.transfersrv_num");
		//Setting &tssrv = srv.lookup("app.transfer");
		//for ( int i = 0; i < star_num_; i++ )
		//{
		//	int id = tssrv[i]["id"];
		//	transfer_id_.push_back( id );
		//	transfer_server_address_.insert( make_pair( id, (const char*) tssrv[i]["addr"]));
		//	transfer_gamed_port_.insert( make_pair( id, (int) tssrv[i]["port"]));
		//	transfer_bind_ip_.insert( make_pair( id, (const char*) tssrv[i]["bind_addr"]));
		//}

		// gamed
		gamed_ip_.clear();
		gamed_port_.clear();
		gamed_port1_.clear();
		gamed_bind_ip_.clear();
		gamed_server_port_.clear();
		gamed_policy_port_.clear();
		gamed_admin_port_.clear();
		gamed_num_ = srv.lookup("app.gamed_num");
		printf("gamed_num:%d\n", gamed_num_);
		if(gamed_num_!=physics_game_num_*physics_region_num)
		{
			printf("Read ServerConfig.cfg File ERROR: gamed_num_!=physics_game_num_*physics_region_num !!!! \r\n");
			return false;
		}
		Setting &gamed = srv.lookup("app.gamed");
		
		for (int i=0; i<gamed_num_; i++) {
			int gamed_id = gamed[i]["id"];
			gamed_ip_.insert(make_pair(gamed_id, (const char *)gamed[i]["addr"]));
			gamed_port_.insert(make_pair(gamed_id, (int) gamed[i]["port"]));
			gamed_port1_.insert(make_pair(gamed_id, (int)gamed[i]["port1"]));
			gamed_bind_ip_.insert(make_pair(gamed_id, (const char*) gamed[i]["bind_addr"]));
			gamed_server_port_.insert(make_pair(gamed_id, (int)gamed[i]["server_port"]));
			gamed_policy_port_.insert(make_pair(gamed_id, (int)gamed[i]["policy_port"]));
			gamed_admin_port_.insert(make_pair(gamed_id, (int)gamed[i]["admin_port"]));
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
		}
 
		Setting &cDB = srv.lookup("app.countrydb");
		countryDB_num_ = srv.lookup("app.countrydb_num");
		for (int i=0; i<countryDB_num_; i++) {
			int db_id = cDB[i]["id"];
			db_Country_addr.insert(make_pair(db_id, (const char*)cDB[i]["addr"]));
			db_Country_port.insert(make_pair(db_id, (int)cDB[i]["port"]));
		}

		Setting &rDB = srv.lookup("app.regiondb");
		regionDB_num_ = srv.lookup("app.regiondb_num");
		for (int i=0; i<regionDB_num_; i++) {
			int db_id = cDB[i]["id"];
			db_Region_addr.insert(make_pair(db_id, (const char*)rDB[i]["addr"]));
			db_Region_port.insert(make_pair(db_id, (int)rDB[i]["port"]));
		}

		Setting &rankDB = srv.lookup("app.rankdb");
		rankDB_num_ = srv.lookup("app.rankdb_num");
		for (int i=0; i<rankDB_num_; i++) {
			int db_id = rankDB[i]["id"];
			db_Rank_addr.insert(make_pair(db_id, (const char*)rankDB[i]["addr"]));
			db_Rank_port.insert(make_pair(db_id, (int)rankDB[i]["port"]));
		}
	}
	catch (ParseException ex)
	{
		printf("Parsing config file %s failed.\n", server_file_.c_str());
		return false;
	}
	catch (FileIOException ex) {
		printf("Read config file %s failed. IOExcetpion.\n", server_file_.c_str());
		return false;
	}
	catch (SettingNotFoundException ex) {
		printf("Read config file %s failed. Setting \"%s\" not found.\n", server_file_.c_str(), ex.getPath());
		return false;
	}

	//syslog.cfg
	try
	{
		srv.readFile(SZSYSLOGCFG);
		gamed_log_start_ = 0;
		star_log_start_ = 0;
		plat_log_start_ = 0;
		fight_log_start_ = 0;

		Setting &gamed = srv.lookup("app.gamed");
		{
			gamed_log_start_			= (int)gamed["log_file_start"];
			gamed_log_dir_				= (const char *)gamed["log_file_dir"];
			gamed_log_name_				= (const char *)(const char *)gamed["log_file_name"];
			gamed_log_stat_level_		= (int)gamed["send_lv"];
			gamed_log_stat_modul_		= (int)gamed["user_mod"];
			gamed_log_stat_modul_val_	= (int)gamed["user_mod_val"];
			gamed_log_stat_addr_		= (const char *)gamed["statsrv_addr"];
			gamed_log_stat_port_		= (const char *)gamed["statsrv_port"];
		}

// 		Setting &star = srv.lookup("app.star");
// 		{
// 
// 			star_log_start_				= (int)star["log_file_start"];
// 			star_log_dir_				= (const char *)star["log_file_dir"];
// 			star_log_name_				= (const char *)star["log_file_name"];
// 			star_log_stat_level_		= (int)star["send_lv"];
// 			star_log_stat_addr_			= (const char *)star["statsrv_addr"];
// 			star_log_stat_port_			= (const char *)star["statsrv_port"];
// 		}
// 
// 		Setting &country = srv.lookup("app.country");
// 		{
// 
// 			country_log_start_				= (int)country["log_file_start"];
// 			country_log_dir_				= (const char *)country["log_file_dir"];
// 			country_log_name_				= (const char *)country["log_file_name"];
// 			country_log_stat_level_			= (int)country["send_lv"];
// 			country_log_stat_addr_			= (const char *)country["statsrv_addr"];
// 			country_log_stat_port_			= (const char *)country["statsrv_port"];
// 		}
// 
// 		Setting &region = srv.lookup("app.region");
// 		{
// 
// 			region_log_start_				= (int)region["log_file_start"];
// 			region_log_dir_				= (const char *)region["log_file_dir"];
// 			region_log_name_				= (const char *)region["log_file_name"];
// 			region_log_stat_level_			= (int)region["send_lv"];
// 			region_log_stat_addr_			= (const char *)region["statsrv_addr"];
// 			region_log_stat_port_			= (const char *)region["statsrv_port"];
// 		}

		Setting &plat = srv.lookup("app.plat");
		{

			plat_log_start_				= (int)plat["log_file_start"];
			plat_log_dir_				= (const char *)plat["log_file_dir"];
			plat_log_name_				= (const char *)plat["log_file_name"];
			plat_log_stat_level_		= (int)plat["send_lv"];
			plat_log_stat_addr_			= (const char *)plat["statsrv_addr"];
			plat_log_stat_port_			= (const char *)plat["statsrv_port"];
		}

// 		Setting &fight = srv.lookup("app.fight");
// 		{
// 
// 			fight_log_start_			= (int)fight["log_file_start"];
// 			fight_log_dir_				= (const char *)fight["log_file_dir"];
// 			fight_log_name_				= (const char *)fight["log_file_name"];
// 			fight_log_stat_level_		= (int)fight["send_lv"];
// 			fight_log_stat_addr_		= (const char *)fight["statsrv_addr"];
// 			fight_log_stat_port_		= (const char *)fight["statsrv_port"];
// 		}
// 
// 		Setting &rank = srv.lookup("app.rank");
// 		{
// 
// 			rank_log_start_			= (int)rank["log_file_start"];
// 			rank_log_dir_				= (const char *)rank["log_file_dir"];
// 			rank_log_name_				= (const char *)rank["log_file_name"];
// 			rank_log_stat_level_		= (int)rank["send_lv"];
// 			rank_log_stat_addr_		= (const char *)rank["statsrv_addr"];
// 			rank_log_stat_port_		= (const char *)rank["statsrv_port"];
// 		}

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

	if(HotCacheDBCfg::Instance().GetHotCacheDBNum()!=dbNum())
	{
		printf("Read HotCacheDB.cfg File ERROR!!!!!\r\n");
		return false;
	}
	return true;
}

int ServerConfig::GetPhysiceRegionByGame(int nGameID)
{
	return (nGameID-1)/physics_game_num_;
}

int ServerConfig::GetPhysicsRegionByLogicRegion(int nRegion)
{
	return m_mapGameRegion[nRegion];
}

int ServerConfig::worlddGamedPort(int id) 
{
	map<int, int>::const_iterator iter = world_gamed_port_.find(id);
	if (iter==world_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

int ServerConfig::transfer_gamed_Port(int id)
{
	map<int, int>::const_iterator iter = transfer_gamed_port_.find(id);
	if (iter==transfer_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

const string ServerConfig::transfer_server_address(int id) 
{
	map<int, string>::const_iterator iter = transfer_server_address_.find(id);
	if (iter==transfer_server_address_.end()) 
		return "";
	return iter->second; 
}

const string ServerConfig::transfer_bind_ip(int id)
{
	map<int, string>::const_iterator iter = transfer_bind_ip_.find(id);
	if (iter==transfer_bind_ip_.end()) 
		return "";
	return iter->second; 
}

int ServerConfig::plat_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = plat_gamed_port_.find(id);
	if (iter==plat_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

const string ServerConfig::plat_server_address(int id) 
{
	map<int, string>::const_iterator iter = plat_server_address_.find(id);
	if (iter==plat_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::plat_bind_ip(int id)
{
	map<int, string>::const_iterator iter = plat_bind_ip_.find(id);
	if (iter==plat_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 

int ServerConfig::fightd_gamed_Port(int id)
{
	map<int, int>::const_iterator iter = fight_gamed_port_.find(id);
	if (iter==fight_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

const string ServerConfig::fightd_server_address(int id)
{
	map<int, string>::const_iterator iter = fight_server_address_.find(id);
	if (iter==fight_server_address_.end()) 
		return "";
	return iter->second; 
}

const string ServerConfig::fightd_bind_ip(int id)
{
	map<int, string>::const_iterator iter = fight_bind_ip_.find(id);
	if (iter==fight_bind_ip_.end()) 
		return "";
	return iter->second; 
}

int ServerConfig::country_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = country_gamed_port_.find(id);
	if (iter==country_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

const string ServerConfig::country_server_address(int id) 
{
	map<int, string>::const_iterator iter = country_server_address_.find(id);
	if (iter==country_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::country_bind_ip(int id)
{
	map<int, string>::const_iterator iter = country_bind_ip_.find(id);
	if (iter==country_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 

int ServerConfig::region_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = region_gamed_port_.find(id);
	if (iter==region_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

const string ServerConfig::region_server_address(int id) 
{
	map<int, string>::const_iterator iter = region_server_address_.find(id);
	if (iter==region_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::regionsrv_bind_ip(int id)
{
	map<int, string>::const_iterator iter = regionsrv_bind_ip_.find(id);
	if (iter==regionsrv_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 


int ServerConfig::rank_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = rank_gamed_port_.find(id);
	if (iter==rank_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

const string ServerConfig::rank_server_address(int id) 
{
	map<int, string>::const_iterator iter = rank_server_address_.find(id);
	if (iter==rank_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::ranksrv_bind_ip(int id)
{
	map<int, string>::const_iterator iter = ranksrv_bind_ip_.find(id);
	if (iter==ranksrv_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 


int ServerConfig::stat_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = stat_gamed_port_.find(id);
	if (iter==stat_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

const string ServerConfig::stat_server_address(int id) 
{
	map<int, string>::const_iterator iter = stat_server_address_.find(id);
	if (iter==stat_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::statsrv_bind_ip(int id)
{
	map<int, string>::const_iterator iter = statsrv_bind_ip_.find(id);
	if (iter==statsrv_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 

int ServerConfig::star_gamed_Port(int id) 
{
	map<int, int>::const_iterator iter = star_gamed_port_.find(id);
	if (iter==star_gamed_port_.end()) 
		return 0;
	return iter->second; 
}; 

int ServerConfig::web_gamed_Port(int id)
{
	map<int, int>::const_iterator iter = web_gamed_port_.find(id);
	if (iter == web_gamed_port_.end()) 
		return 0;
	return iter->second; 
}

const string ServerConfig::web_server_address(int id)
{
	map<int, string>::const_iterator iter = web_server_address_.find(id);
	if (iter == web_server_address_.end()) 
		return "";
	return iter->second; 
}

const string ServerConfig::star_server_address(int id) 
{
	map<int, string>::const_iterator iter = star_server_address_.find(id);
	if (iter==star_server_address_.end()) 
		return "";
	return iter->second; 
}; 

const string ServerConfig::star_bind_ip(int id)
{
	map<int, string>::const_iterator iter = star_bind_ip_.find(id);
	if (iter==star_bind_ip_.end()) 
		return "";
	return iter->second; 
}; 

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

int ServerConfig::gamedPort1(int nid)
{
	map<int, int>::const_iterator iter = gamed_port1_.find(nid);
	if (iter==gamed_port1_.end())
		return 0;
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

int ServerConfig::getregionid(int nindex)
{
	if (nindex<0 || nindex>=(int)region_id_.size())
	{
		return -1;
	}
	return region_id_[nindex];
}

bool ServerConfig::IsSubRegionInGroup(int region)
{
	map<int,int>::iterator iter = m_mapRegionToGroup.find(region);
	if (iter != m_mapRegionToGroup.end())
	{
		int groupid = iter->second;
		map<int,vector<int> >::iterator itergroup = m_mapGroupRegion.find(groupid);
		if (itergroup == m_mapGroupRegion.end())
		{
			return false;
		}
		return (region != itergroup->second[0]);
	}
	return false;
}

int ServerConfig::getgroupidbyregion(int regionid)
{
	map<int,int>::iterator iter = m_mapRegionToGroup.find(regionid);
	if (iter != m_mapRegionToGroup.end())
	{
		return iter->second;
	}
	return -1;
}

int ServerConfig::getmapregion(int regionid)
{
	// 不存在映射表，返回自己
	int ngroupid = getgroupidbyregion(regionid);
	if (-1 == ngroupid)
	{
		return regionid;
	}
	// 是映射表中的，检测一下是否存在，也是为了校验有没有配错
	map<int,vector<int> >::iterator iter = m_mapGroupRegion.find(ngroupid);
	if (iter !=m_mapGroupRegion.end())
	{
		if (iter->second.size()>0)
			return ngroupid;
	}
	return regionid;
}

int ServerConfig::getRegionResetFlag(int nRegionID)
{
	map<int,int>::iterator iter;
	iter = m_mapRegionResetFlag.find(nRegionID);
	if(iter == m_mapRegionResetFlag.end())
		return 0;
	return iter->second;
}


bool ServerConfig::regionisequal(int region1,int region2)
{
	if (region1 == region2)
	{
		return true;
	}
	int ntransregion1 = getmapregion(region1);
	if (ntransregion1 == -1)
	{
		return false;
	}
	return ntransregion1 == getmapregion(region2);
}

int ServerConfig::getgroupsize(int ngroupid)
{
	map<int,vector<int> >::iterator iter = m_mapGroupRegion.find(ngroupid);
	if (iter == m_mapGroupRegion.end())
	{
		return 0;
	}
	return iter->second.size();
}

int ServerConfig::getregionfromgroup(int ngroupid,int nindex)
{
	map<int,vector<int> >::iterator iter = m_mapGroupRegion.find(ngroupid);
	if (iter == m_mapGroupRegion.end())
	{
		return -1;
	}
	if (nindex<0 || nindex>= (int)iter->second.size())
	{
		return -1;
	}
	return iter->second[nindex];
}

bool ServerConfig::findregion_id(int id)
{
	vector<int>::iterator iter;
	for (iter =region_id_.begin();iter!=region_id_.end();++iter)
	{
		if (*iter == id)
		{
			return true;
		}
	}
	return false;
}
