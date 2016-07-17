#ifndef _UserLogin_H_
#define _UserLogin_H_
#pragma once

#include "BaseEvent.h"

/*

自定义EventHandler:
1，继承自CBaseEvent
2，使用BIND_1_EVENT/BIND_2_EVENT等注册事件
3，initialEventProcessors()中创建实例

*/

class User;

class UserLogin : public CBaseEvent
{
public:
    UserLogin(){}
    ~UserLogin(){}

	static void createInstance(GameEventHandler* eh)
	{
		instance_ = new UserLogin();
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_USER_LOGIN,
			(ProcessRoutine) UserLogin::handle_);
		eh->getEventHandler()->registHandler(EVENT_USER_REGIST,
			(ProcessRoutine) UserLogin::handle_);

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
    void handle(Event* e);
    void handle_login(Event* e);
    void handle_regist(Event* e);

    string succContent(User* user, const string& sid, int isnew);
    string errorContent(const string& msg, int errorCode, const string& sid);

	static UserLogin* instance_;
} ;

#endif
