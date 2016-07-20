#ifndef _UPDATEWORKINGSTATUS_H_
#define _UPDATEWORKINGSTATUS_H_

#pragma once
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"

class UpdateWorkingStatus 
{
public:
	UpdateWorkingStatus()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	~UpdateWorkingStatus() {}

	static void createInstance(WorldEventHandler* eh)
	{
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_UPDATE_WORKING_STATUS,
			(ProcessRoutine)UpdateWorkingStatus::handle_);
	}

	static UpdateWorkingStatus* getInstance()
	{
		static UpdateWorkingStatus instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		getInstance()->handle(e);
	}

	static void addEvent(EventQueue* eq, int state, int gid);

private:
	void handle(Event* e);
	void handle_GW_Disconn(Event* e);
	void handle_GW_Conn(Event* e);
	void handle_GW_Sync(Event* e);
private:
	WorldEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
};

#endif
