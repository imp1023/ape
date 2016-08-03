#ifndef _SERVERCONFIG_H_
#define _SERVERCONFIG_H_
#pragma once
#include <stdlib.h>
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

const char SZSYSLOGCFG[] = "syslog.cfg";

using namespace std;
using namespace libconfig;

class ServerConfig
{
public:
	ServerConfig(const char* server_file);
public:
	~ServerConfig(void);

	bool loadFromFile();

	int worlddNum() { return worldd_num_; }
	int worlddId(int id) {return world_id_[id];};
	const string worlddIp(int id);
	const string worlddBindIp(int id);
	int worlddGamedPort(int id);
	int worlddWebPort(int id);


	int gamedNum() {return gamed_num_;}
	int physicsGameNum() {return physics_game_num_;}
	const string gamedIp(int nid);
	int gamedPort(int nid);
	int gamedPort1(int nid);
	const string gamedBindIp(int nid);
	int gamedServerPort(int gid) {return gamed_server_port_[gid];}
	int gamedAdminPort(int gid) {return gamed_admin_port_[gid];}
	int gamedPolicyPort(int gid) {return gamed_policy_port_[gid];}

	int star_num() { return star_num_; };
	int starID( int index )	{ return star_id_[index]; }; 
	int star_gamed_Port(int id); 
	const string star_server_address(int id) ; 
	const string star_bind_ip(int id);

	int country_num() { return country_num_; };
	int countryID( int index )	{ return country_id_[index]; }; 
	int country_gamed_Port(int id); 
	const string country_server_address(int id) ; 
	const string country_bind_ip(int id);

	int regionsrv_num() { return regionsrv_num_; };
	int regionsrvID( int index )	{ return regionsrv_id_[index]; }; 
	int region_gamed_Port(int id); 
	const string region_server_address(int id) ; 
	const string regionsrv_bind_ip(int id);

	int plat_num() { return plat_num_; };
	int platID( int index )	{ return plat_id_[index]; }; 
	int plat_gamed_Port(int id); 
	const string plat_server_address(int id) ; 
	const string plat_bind_ip(int id);

	int fightd_num() { return fight_num_; };
	int fightdID( int index )	{ return fight_id_[index]; }; 
	int fightd_gamed_Port(int id); 
	const string fightd_server_address(int id) ; 
	const string fightd_bind_ip(int id);

	int transfer_num() { return transfer_num_; };
	int transferID( int index )	{ return transfer_id_[index]; }; 
	int transfer_gamed_Port(int id); 
	const string transfer_server_address(int id) ; 
	const string transfer_bind_ip(int id);

	int ranksrv_num() { return ranksrv_num_; };
	int ranksrvID( int index )	{ return ranksrv_id_[index]; }; 
	int rank_gamed_Port(int id); 
	const string rank_server_address(int id) ; 
	const string ranksrv_bind_ip(int id);

	int statsrv_num() { return statsrv_num_; };
	int statsrvID( int index )	{ return statsrv_id_[index]; }; 
	int stat_gamed_Port(int id); 
	const string stat_server_address(int id) ; 
	const string statsrv_bind_ip(int id);

	int web_num() { return web_num_; };
	int webID( int index )	{ return web_id_[index]; }; 
	int web_gamed_Port(int id); 
	const string web_server_address(int id) ; 

	//获得game服务器所属的物理分区，分区从0开始
	int GetPhysiceRegionByGame(int nGameID);
	int GetPhysicsRegionByLogicRegion(int nRegion);

	//分服信息
	int region_num(){return region_num_;};
	//const string region_name(int id);
	bool findregion_id(int id);

	// 分区的数量
	int getregionsize(){return region_id_.size();};
	int getregionid(int nindex);
	// 得到区映射表中对应的区号，不在映射表，就是本身
	int getmapregion(int regionid);

	// 查找区ID对应的分组ID
	int getgroupidbyregion(int regionid);

	// 检测是否是合区的从属区
	bool IsSubRegionInGroup(int region);

	// 分组中存在几个区
	int getgroupsize(int ngroupid);

	// 获得分组中得区
	int getregionfromgroup(int ngroupid,int nindex);

	// 判断两区是否可见
	bool regionisequal(int region1,int region2);
	
	//获得分区重置标记
	int getRegionResetFlag(int nRegionID);

	int countryDBNum(){return countryDB_num_;}
	const string dbCountryAddr(int nID){return db_Country_addr[nID];}
	int dbCountryPort(int nID){return db_Country_port[nID]; }

	int regionDBNum(){return regionDB_num_;}
	const string dbRegionAddr(int nID){return db_Region_addr[nID];}
	int dbRegionPort(int nID){return db_Region_port[nID]; }

	int rankDBNum(){return rankDB_num_;}
	const string dbRankAddr(int nID){return db_Rank_addr[nID];}
	int dbRankPort(int nID){return db_Rank_port[nID]; }


	int dbNum() {return db_num_;}
	const string& dbUserAddr1(int gid) {return db_user_addr_1[gid];}
	int dbUserPort1(int gid) {return db_user_port_1[gid];}
	const string& dbPlatAddr1(int gid) {return db_plat_addr_1[gid];}
	int dbPlatPort1(int gid) {return db_plat_port_1[gid];}
	const string& dbUserAddr2(int gid) {return db_user_addr_2[gid];}
	int dbUserPort2(int gid) {return db_user_port_2[gid];}
	const string& dbPlatAddr2(int gid) {return db_plat_addr_2[gid];}
	int dbPlatPort2(int gid) {return db_plat_port_2[gid];}

	int proxyNum() {return proxy_num_;}
	const string proxyIp(int pid) {return proxy_addr[pid];}
	int proxyBindPort1(int pid) {return proxy_port_1[pid];}
	int proxyBindPort2(int pid) {return proxy_port_2[pid];}
	int proxyPolicyPort(int pid) {return proxy_policy_port[pid];}


	//日志系统相关函数
	bool		 IsGameLogStart(){return gamed_log_start_>0;}
	const string GetGameLogDir() {return gamed_log_dir_;}
	const string GetGameLogName(){return gamed_log_name_;}
	int			 GetGameLogStatLv(){return gamed_log_stat_level_;}
	int			 GetGameLogStatModul(){return gamed_log_stat_modul_;}
	int			 GetGameLogStatModulVal(){return gamed_log_stat_modul_val_;}
	const string GetGameLog2SrvAddr(){return gamed_log_stat_addr_;}
	const string GetGameLog2SrvPort(){return gamed_log_stat_port_;}

	bool		 IsStarLogStart(){return star_log_start_>0;}
	const string GetStarLogDir() {return star_log_dir_;}
	const string GetStarLogName(){return star_log_name_;}
	int			 GetStarLogStatLv(){return star_log_stat_level_;}
	const string GetStarLog2SrvAddr(){return star_log_stat_addr_;}
	const string GetStarLog2SrvPort(){return star_log_stat_port_;}

	bool		 IsCountryLogStart(){return country_log_start_>0;}
	const string GetCountryLogDir() {return country_log_dir_;}
	const string GetCountryLogName(){return country_log_name_;}
	int			 GetCountryLogStatLv(){return country_log_stat_level_;}
	const string GetCountryLog2SrvAddr(){return country_log_stat_addr_;}
	const string GetCountryLog2SrvPort(){return country_log_stat_port_;}

	bool		 IsRegionLogStart(){return region_log_start_>0;}
	const string GetRegionLogDir() {return region_log_dir_;}
	const string GetRegionLogName(){return region_log_name_;}
	int			 GetRegionLogStatLv(){return region_log_stat_level_;}
	const string GetRegionLog2SrvAddr(){return region_log_stat_addr_;}
	const string GetRegionLog2SrvPort(){return region_log_stat_port_;}

	bool		 IsPlatLogStart(){return plat_log_start_>0;}
	const string GetPlatLogDir() {return plat_log_dir_;}
	const string GetPlatLogName(){return plat_log_name_;}
	int			 GetPlatLogStatLv(){return plat_log_stat_level_;}
	const string GetPlatLog2SrvAddr(){return plat_log_stat_addr_;}
	const string GetPlatLog2SrvPort(){return plat_log_stat_port_;}


	bool		 IsFightdLogStart(){return fight_log_start_>0;}
	const string GetFightdLogDir() {return fight_log_dir_;}
	const string GetFightdLogName(){return fight_log_name_;}
	int			 GetFightdLogStatLv(){return fight_log_stat_level_;}
	const string GetFightdLog2SrvAddr(){return fight_log_stat_addr_;}
	const string GetFightdLog2SrvPort(){return fight_log_stat_port_;}

	bool		 IsRankLogStart(){return rank_log_start_>0;}
	const string GetRankLogDir() {return rank_log_dir_;}
	const string GetRankLogName(){return rank_log_name_;}
	int			 GetRankLogStatLv(){return rank_log_stat_level_;}
	const string GetRankLog2SrvAddr(){return rank_log_stat_addr_;}
	const string GetRankLog2SrvPort(){return rank_log_stat_port_;}
private:

	string version_;

	map<int, string> proxy_ip_;
	map<int, int> proxy_port_1_;
	map<int, int> proxy_port_2_;
	map<int, int> proxy_policy_port_;
	int worldd_num_;
	vector<int> world_id_;
	map<int, string> world_bind_ip_;
	map<int, string> world_server_address_;
	map<int, int> world_gamed_port_;
	map<int, int> world_web_server_port_;

	int physics_game_num_;
	int gamed_num_;
	map<int, string> gamed_ip_;
	map<int, int> gamed_port_;
	map<int, int> gamed_port1_;
	map<int, string> gamed_bind_ip_;
	map<int, int> gamed_server_port_;
	map<int, int> gamed_admin_port_;
	map<int, int> gamed_policy_port_;

	//star srv
	int star_num_;
	vector<int> star_id_;
	map<int, string> star_bind_ip_;
	map<int, string> star_server_address_;
	map<int, int> star_gamed_port_;

	//country srv
	int country_num_;
	vector<int> country_id_;
	map<int, string> country_bind_ip_;
	map<int, string> country_server_address_;
	map<int, int> country_gamed_port_;

	//region srv
	int regionsrv_num_;
	vector<int> regionsrv_id_;
	map<int, string> regionsrv_bind_ip_;
	map<int, string> region_server_address_;
	map<int, int> region_gamed_port_;

	//plat srv
	int plat_num_;
	vector<int> plat_id_;
	map<int, string> plat_bind_ip_;
	map<int, string> plat_server_address_;
	map<int, int> plat_gamed_port_;

	//fight srv
	int fight_num_;
	vector<int> fight_id_;
	map<int, string> fight_bind_ip_;
	map<int, string> fight_server_address_;
	map<int, int> fight_gamed_port_;

	//transfer srv
	int transfer_num_;
	vector<int> transfer_id_;
	map<int, string> transfer_bind_ip_;
	map<int, string> transfer_server_address_;
	map<int, int> transfer_gamed_port_;

	//rank srv
	int ranksrv_num_;
	vector<int> ranksrv_id_;
	map<int, string> ranksrv_bind_ip_;
	map<int, string> rank_server_address_;
	map<int, int> rank_gamed_port_;

	//stat srv
	int statsrv_num_;
	vector<int> statsrv_id_;
	map<int, string> statsrv_bind_ip_;
	map<int, string> stat_server_address_;
	map<int, int> stat_gamed_port_;


	// 分服信息
	int region_num_; //服务区总数
	int group_num_;	 //分组（合服）信息
	vector<int> region_id_;
	// 区分组列表
	map<int,vector<int> > m_mapGroupRegion;
	// 区到分组的映射表
	map<int,int> m_mapRegionToGroup;
	//每个分区对应的game分区
	map<int,int> m_mapGameRegion;	
	//分区重置标记，用于合区后的数据重置
	map<int,int> m_mapRegionResetFlag;

	int countryDB_num_;
	map<int, string> db_Country_addr;
	map<int, int>    db_Country_port;

	int regionDB_num_;
	map<int, string> db_Region_addr;
	map<int, int>    db_Region_port;
	
	int rankDB_num_;
	map<int, string> db_Rank_addr;
	map<int, int>    db_Rank_port;

	int db_num_;
	map<int, string> db_user_addr_1;
	map<int, int>    db_user_port_1;
	map<int, string> db_plat_addr_1;
	map<int, int>    db_plat_port_1;
	map<int, string> db_user_addr_2;
	map<int, int>    db_user_port_2;
	map<int, string> db_plat_addr_2;
	map<int, int>    db_plat_port_2;

	//web srv
	int web_num_;
	vector<int> web_id_;
	map<int, string> web_server_address_;
	map<int, int> web_gamed_port_;

	int proxy_num_;
	map<int, string> proxy_addr;
	map<int, int> proxy_port_1;
	map<int, int> proxy_port_2;
	map<int, int> proxy_policy_port;

	//log stat args:
	string	gamed_log_dir_;
	string	gamed_log_name_;
	int		gamed_log_start_;
	int		gamed_log_stat_level_;
	int	    gamed_log_stat_modul_;
	int		gamed_log_stat_modul_val_;
	string	gamed_log_stat_addr_;
	string	gamed_log_stat_port_;

	string	star_log_dir_;
	string	star_log_name_;
	int		star_log_start_;
	int		star_log_stat_level_;
	string	star_log_stat_addr_;
	string	star_log_stat_port_;

	string	country_log_dir_;
	string	country_log_name_;
	int		country_log_start_;
	int		country_log_stat_level_;
	string	country_log_stat_addr_;
	string	country_log_stat_port_;

	string	region_log_dir_;
	string	region_log_name_;
	int		region_log_start_;
	int		region_log_stat_level_;
	string	region_log_stat_addr_;
	string	region_log_stat_port_;

	string	plat_log_dir_;
	string	plat_log_name_;
	int		plat_log_start_;
	int		plat_log_stat_level_;
	string	plat_log_stat_addr_;
	string	plat_log_stat_port_;

	string	fight_log_dir_;
	string	fight_log_name_;
	int		fight_log_start_;
	int		fight_log_stat_level_;
	string	fight_log_stat_addr_;
	string	fight_log_stat_port_;

	string	rank_log_dir_;
	string	rank_log_name_;
	int		rank_log_start_;
	int		rank_log_stat_level_;
	string	rank_log_stat_addr_;
	string	rank_log_stat_port_;

	string server_file_;
	Config srv;
};

#endif
