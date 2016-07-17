#ifndef _SERVERCONFIG_H_
#define _SERVERCONFIG_H_
#pragma once
#include <vector>
#include <string>
#include <set>
#include "../common/const_def.h"

#ifdef _WIN32
#pragma warning(disable: 4290)
#include <libconfig.hpp>
#else
#include <libconfig.h++>
#endif

using namespace std;
using namespace libconfig;

class ServerConfig
{
protected:
    ServerConfig();
    ~ServerConfig(void);

    bool loadFromFile();

public:
    static ServerConfig& Instance() {return m_ServerConfig;}

	int worlddNum() { return worldd_num_; }
	int worlddId(int id) {return world_id_[id];};
	const string worlddIp(int id);
	const string worlddBindIp(int id);
	int worlddGamedPort(int id);
	int worlddWebPort(int id);
	int worlddLocaldbPort(int wid) {return worldd_localdb_port_[wid];}

	int gamedNum() {return gamed_num_;}
	const string gamedIp(int nid);
	int gamedPort(int nid);
	const string gamedBindIp(int nid);
	int gamedServerPort(int gid) {return gamed_server_port_[gid];}
    int gamedLocaldbPort(int gid) {return gamed_localdb_port_[gid];}
	int gamedPolicyPort(int gid) {return gamed_policy_port_[gid];}

	int dbNum() {return db_num_;}
	const string& dbUserAddr1(int gid) {return db_user_addr_1[gid];}
	int dbUserPort1(int gid) {return db_user_port_1[gid];}
	const string& dbPlatAddr1(int gid) {return db_plat_addr_1[gid];}
	int dbPlatPort1(int gid) {return db_plat_port_1[gid];}
	const string& dbUserAddr2(int gid) {return db_user_addr_2[gid];}
	int dbUserPort2(int gid) {return db_user_port_2[gid];}
	const string& dbPlatAddr2(int gid) {return db_plat_addr_2[gid];}
	int dbPlatPort2(int gid) {return db_plat_port_2[gid];}
	const string& dbUserAddrOld(int gid) {return db_user_addr_old[gid];}
	int dbUserPortOld(int gid) {return db_user_port_old[gid];}
	const string& dbUserBack(int gid) {return db_user_addr_back[gid];}
	int dbUserPortback(int gid) {return db_user_port_back[gid];}
	const string& dbPlatBack(int gid) {return db_plat_addr_back[gid];}
	int dbPlatPortback(int gid) {return db_plat_port_back[gid];}

	int proxyNum() {return proxy_num_;}
	const string proxyIp(int pid) {return proxy_addr[pid];}
	int proxyBindPort1(int pid) {return proxy_port_1[pid];}
	int proxyBindPort2(int pid) {return proxy_port_2[pid];}
	int proxyPolicyPort(int pid) {return proxy_policy_port[pid];}

    int regionNum() {return region_num_;}
    int dbId(int region) {return region_dbid[region];}
    int newregionId(int region) {return region_newregionid[region];}
    int gameregion(int gameid) {return game_newregionid[gameid];}
    int gameid(int region) {return region_gameid[region];}

	//日志系统相关函数
	bool		 IsGameLogStart(){return gamed_log_start_>0;}
	const string GetGameLogDir() {return gamed_log_dir_;}
	const string GetGameLogName(){return gamed_log_name_;}
	int			 GetGameLogStatLv(){return gamed_log_stat_level_;}
	int			 GetGameLogStatModul(){return gamed_log_stat_modul_;}
	int			 GetGameLogStatModulVal(){return gamed_log_stat_modul_val_;}
	const string GetGameLog2SrvAddr(){return gamed_log_stat_addr_;}
	const string GetGameLog2SrvPort(){return gamed_log_stat_port_;}

	bool		 IsWorldLogStart(){return worldd_log_start_>0;}
	const string GetWorldLogDir() {return worldd_log_dir_;}
	const string GetWorldLogName(){return worldd_log_name_;}
	int			 GetWorldLogStatLv(){return worldd_log_stat_level_;}
	int			 GetWorldLogStatModul(){return worldd_log_stat_modul_;}
	int			 GetWorldLogStatModulVal(){return worldd_log_stat_modul_val_;}
	const string GetWorldLog2SrvAddr(){return worldd_log_stat_addr_;}
	const string GetWorldLog2SrvPort(){return worldd_log_stat_port_;}

    LOCALE_TYPE  GetServerLocale() const {return locale_;}
    CHANNEL_TYPE GetServerChannel() const {return channel_;}
    bool         IsTestServer() const {return test_;}
	
private:
	int worldd_num_;
	vector<int> world_id_;
	map<int, string> world_bind_ip_;
	map<int, string> world_server_address_;
	map<int, int> world_gamed_port_;
	map<int, int> world_web_server_port_;
	map<int, int> worldd_localdb_port_;

	int gamed_num_;
	map<int, string> gamed_ip_;
	map<int, int> gamed_port_;
	map<int, string> gamed_bind_ip_;
	map<int, int> gamed_server_port_;
    map<int, int> gamed_localdb_port_;
	map<int, int> gamed_policy_port_;

	int db_num_;
	map<int, string> db_user_addr_1;
	map<int, int>    db_user_port_1;
	map<int, string> db_plat_addr_1;
	map<int, int>    db_plat_port_1;
	map<int, string> db_user_addr_2;
	map<int, int>    db_user_port_2;
	map<int, string> db_plat_addr_2;
	map<int, int>    db_plat_port_2;
	map<int, string> db_user_addr_old;
	map<int, int>	 db_user_port_old;
	map<int, string> db_user_addr_back;
	map<int, int>    db_user_port_back;
	map<int, string> db_plat_addr_back;
	map<int, int>    db_plat_port_back;

	int proxy_num_;
	map<int, string> proxy_addr;
	map<int, int> proxy_port_1;
	map<int, int> proxy_port_2;
	map<int, int> proxy_policy_port;

    int region_num_;
    map<int, int> region_newregionid;
    map<int, int> region_dbid;
    map<int, int> region_gameid;
    map<int, int> game_newregionid;

	//log stat args:
	string	gamed_log_dir_;
	string	gamed_log_name_;
	int		gamed_log_start_;
	int		gamed_log_stat_level_;
	int	    gamed_log_stat_modul_;
	int		gamed_log_stat_modul_val_;
	string	gamed_log_stat_addr_;
	string	gamed_log_stat_port_;

	string	worldd_log_dir_;
	string	worldd_log_name_;
	int		worldd_log_start_;
	int		worldd_log_stat_level_;
	int	    worldd_log_stat_modul_;
	int		worldd_log_stat_modul_val_;
	string	worldd_log_stat_addr_;
	string	worldd_log_stat_port_;

    LOCALE_TYPE  locale_;
    CHANNEL_TYPE channel_;
    bool         test_;

	Config srv;

    static const char* szConfigFile;
    static ServerConfig m_ServerConfig;

#ifdef _WIN32
    void copyFileForWindows();
    static const char* szConfigFile_Win32;
#endif
};

#endif
