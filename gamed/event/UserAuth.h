#ifndef _USERAUTH_H_
#define _USERAUTH_H_
#pragma once
#include "../../event/EventDefine.h"
#include "../GameEventHandler.h"
#include "MessageDef.h"

class EventQueue;

class UserAuth {
public:
	UserAuth()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	~UserAuth() {}

	static void createInstance(GameEventHandler* eh)
	{
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_USER_AUTH,
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

	static void addEvent(EventQueue* eq, int64 uid, int64 secret, int fd);
private:
	void handle(Event* e);
	void handle_CG_Req(Event* e);
private:
	GameEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
};

#endif
