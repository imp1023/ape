#pragma once

#include <log4cxx/logger.h>
#include "../event/EventHandler.h"

class Event;
class EventQueue;
class CountryDataHandler;
class CountryNetHandler;
class CountryRgnDatHandler;
class DB_C_City;
class CCity;
class DB_C_User;

class CountryEventHandler
{
public:
	CountryEventHandler(EventQueue *eq, CountryDataHandler *dh, CountryNetHandler *nh, int nid);
	~CountryEventHandler();

public:
	void start();
	EventHandler* getEventHandler(){ return eh_; }
	CountryDataHandler* getDataHandler() { return dh_; }
	EventQueue* getEventQueue() { return eq_; }
	int	 GetCountrySrvID() {return nid_;}
	// Helper
	bool sendEventToGamed(Event *e, int gid);
	bool sendEventToAllGamed(Event *e);
	void sendFdString(int fd, const string &content);

	int getGamedIdByUserId(int64 uid);

	inline time_t getReversion() {return eh_->getReversion();}
	int GetSrvID() {return nid_;}

public:
	void SendCountryLiteInfo(int nGameID = -1);
	void SendCityBuffInfo(int nGameID = -1);
	void SendBloodBtlInfo(int nSelRegion = -1, int nGameID = -1,int nBroadAtkCity = 0, int nBroadDefCity = 0, bool bOver=false,map<int,bool>* pMapBroacdCity = NULL);	//发送血战信息
	//发送官员信息到该国家的所有玩家
	void SendOfficerRightInfo2CountryUser(int countryId = 0,int nSelRegion = -1,int nGameID = -1,E_Officer_Fill_Type fillType = E_Officer_Fill_ALL);
	//发送官员信息到单个玩家
	void SendOfficerRightInfo2User(CountryRgnDatHandler* pRDH,int countryId = 0,int64 nUid = 0,E_Officer_Fill_Type fillType = E_Officer_Fill_ALL);
	void SendOfficerRightUsed(CountryRgnDatHandler* pRDH,CCity* pCity,int64 nUid = 0);
	void SendCountryUserLiteInfo(CountryRgnDatHandler* pRDH,DB_C_User* pDBCUser);

	void BroadCastMsg_GuildBtlSessionOver(int nRegion,string strGuildName,string strLeaderNmae);	//本届工会战结束

	void SendBossBtlLiteInfo(int nGameID = -1);

public:
	inline void postBackEvent(Event* e)
	{
		eh_->postBackEvent(e);
	}

public:
private:
	void initialEventProcessors();

	EventQueue* eq_;
	CountryDataHandler* dh_;
	CountryNetHandler* nh_;
	EventHandler* eh_;
	int nid_;
	log4cxx::LoggerPtr logger_;
};

