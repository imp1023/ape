#ifndef _USERLOGIN_H_
#define _USERLOGIN_H_

#pragma once
#include "../../common/json-util.h"
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"
class UserLogin_Req;

class UserLogin {
public:
	UserLogin()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	~UserLogin() {}

	static void createInstance(WorldEventHandler* eh)
	{
		instance_ = new UserLogin();
		instance_->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_USER_LOGIN,
			(ProcessRoutine)UserLogin::handle_);
		//eh->getEventHandler()->registHandler(EVENT_USER_PRELOGIN,
		//	(ProcessRoutine)UserLogin::handle_);

	}

	static UserLogin* getInstance()
	{
		return instance_;
	}

	static void handle_(Event* e)
	{
		UserLogin::getInstance()->handle(e);
	}

	static void addEvent(EventQueue* eq, int fd, int siteid, string const& platid,
		string const& name, string const& head, int gender, vector<string>& friends,
		int isYellowDmd, int isYellowDmdYear, int levelYellowDmd, string sid,int nCity,
		string strVIA,int region,bool isRecall,bool isHighVip,bool isApp51Act,int nCustomFlag,
		string strTaskMarketId,string strChannel,int nBackFlowFlag,bool isSuperVip,unsigned int,unsigned int,unsigned int,unsigned int);

private:
	void handle(Event* e);
	void handle_WW_Req(Event* e);
	void handle_GW_Rsp(Event* e);
	void buildUserLoginJson(int64 uid, const string& secret,const string& server, int port, 
		int port1, string sid, int nRegion,const string& strMainLoad, const string& strVersion,
		int nlogintimes,int nlvl, bool bIsNewPlayer,string strExtras, Json::Value& json);
	
	static void TokenizeChannel(string& strChannel,UserLogin_Req* req);
private:
	WorldEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
	static UserLogin* instance_;
};

#endif
