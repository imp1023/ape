#ifndef _UserLogin_H_
#define _UserLogin_H_

#pragma once
#include "../../event/EventDefine.h"
#include "../GameEventHandler.h"
#include "../GameDataHandler.h"
#include "../../logic/User.h"

class User;
//enum LoadStatus;

class UserLogin
{
public:

    UserLogin()
    {
        logger_ = log4cxx::Logger::getLogger("EventHelper");
    }

    ~UserLogin()
    {
    }

    static void createInstance(GameEventHandler* eh)
    {
        instance_ = new UserLogin();
        instance_->eh_ = eh;
        eh->getEventHandler()->registHandler(EVENT_USER_LOGIN,
                                             (ProcessRoutine) UserLogin::handle_);
        eh->getEventHandler()->registHandler(EVENT_USER_LOGOUT,
                                             (ProcessRoutine) UserLogin::handle_);
		//eh->getEventHandler()->registHandler(EVENT_USER_PRELOGIN,
		//									(ProcessRoutine) UserLogin::handle_);

    }

    static UserLogin* getInstance()
    {
        return instance_;
    }

    static void handle_(Event* e)
    {
        UserLogin::getInstance()->handle(e);
    }
private:
    void HandleUserLogin(Event* e);
    void HandleUserLogout(Event* e);
//	void HandleUserPreLogin(Event* e);
private:
    User* processUserLogin(int64 uid,const string& platid, int siteid, const string& name,
                           const string& profile_link, int gender, vector<string> friends_platid,
                           bool isYellowDmd, bool isYellowDmdYear, int lvlYellowDmd, LoadStatus& status,int region,int nCity,bool bIsNewUser,string strVIA,bool isHighYellowDmd,string strChannel,bool isHighDmdYear, int, int, int, int nHighBlueYearTime,
						   int nID, int nName, int nType, string strSku);
    void handle(Event* e);
	//void handle_WG_UserPreLogin_Psp(Event* e);
	//void handle_WG_UserPreLogin(Event* e);
    void handle_WG_UserLogin(Event* e);
    void PushOnlineStatus(User* pUser, int64 uid);
	//void PushUpdateFriendCache(User* pUser, const string& platid, int64 uid, bool isplatfriend = true);

	// 分组列表中得区校正（改区不在合租分区列表中，返回改区；
	// 在分组表中，该区有角色，直接返回该区;该区没角色，返回主区;主区没角色，返回有角色的区，都没角色，返回主区）
	//int CorrectRegion(DB_PlatidInfoList* pPlatidList,int nRegion);

	//void RereshPlatInfo(string platID,DB_PlatidInfoList* pPlatidInfoList,User *pUser);
private:
    GameEventHandler* eh_;
    log4cxx::LoggerPtr logger_;
    static UserLogin* instance_;
} ;

#endif
