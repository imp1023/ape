#ifndef _UPDATEWORKINGSTATUS_H_
#define _UPDATEWORKINGSTATUS_H_

#pragma once
#include "../../event/EventDefine.h"
#include "../GameEventHandler.h"

class UpdateWorkingStatus {
public:
	UpdateWorkingStatus()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	~UpdateWorkingStatus() {}

	static void createInstance(GameEventHandler* eh)
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

	void handle_WG_Disconn(Event* e);
	void handle_WG_Sync(Event* e);
	void handle_WG_Fin(Event* e);

	void handle_HG_Disconn(Event* e);
	void handle_HG_Sync(Event* e);
	void handle_HG_Fin(Event* e);

	void handle_UG_Disconn(Event* e);
	void handle_UG_Sync(Event* e);
	void handle_UG_Fin(Event* e);

	void handle_SG_Disconn(Event* e);
	void handle_SG_Sync(Event* e);
	void handle_SG_Fin(Event* e);

private:
	GameEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
};

#endif
