#ifndef _USERAUTH_H_
#define _USERAUTH_H_
#pragma once

#include "BaseEvent.h"

class UserAuth : public CBaseEvent
{
public:
	UserAuth(){}
	~UserAuth(){}

	static void createInstance(GameEventHandler* eh)
	{
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_USER_AUTH,
			(ProcessRoutine)UserAuth::handle_);
		eh->getEventHandler()->registHandler(EVENT_USER_DISCONNECT,
			(ProcessRoutine)UserAuth::handle_);		
	}

	static UserAuth* getInstance()
	{
		static UserAuth instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		UserAuth::getInstance()->handle(e);
	}

	static void addAuthEvent(EventQueue* eq, int64 uid, int64 secret, int fd);

private:
	void handle(Event* e);
    void handle_auth(Event* e);
    void handle_disconnect(Event* e);
};

#endif
