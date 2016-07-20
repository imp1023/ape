#ifndef _SENDREMOTEUSER_H_
#define _SENDREMOTEUSER_H_

#pragma once
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"

class EventQueue;

class SendRemoteUser {
public:
	SendRemoteUser()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	~SendRemoteUser() {}

	static void createInstance(WorldEventHandler* eh)
	{
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_SEND_REMOTE_USER,
			(ProcessRoutine)SendRemoteUser::handle_);
	}

	static SendRemoteUser* getInstance()
	{
		static SendRemoteUser instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		getInstance()->handle(e);
	}

private:
	void handle(Event* e);
	void handle_GW_Req(Event* e);
private:
	WorldEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
};

#endif
