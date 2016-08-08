#ifndef _DEALADMINEVENT_H_ 
#define _DEALADMINEVENT_H_  

#pragma once
#include "../WorldEventHandler.h"
#include "../../event/EventDefine.h"

class DealAdminEvent
{
public:
	DealAdminEvent()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}
	~DealAdminEvent() {}

	static void createInstance(WorldEventHandler* eh) 
	{ 
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDEXP,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_CANATTACK,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDCASH,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_OPT,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDHONOR,
			((ProcessRoutine)DealAdminEvent::handle_));


		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETLEVEL,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANCHAT,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANLOGIN,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_RELOAD,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_AD_SET,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANDWGUSER,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_CLEARDEFENSE,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETNEWCOMERGIFT,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETCONTINUELANDDAY,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETPLATINVITEID,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_GETBLDCNT,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_RELOADCHICKENRANK,
			((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SITECREDITRELOAD,
			((ProcessRoutine)DealAdminEvent::handle_));
	}

	static DealAdminEvent* getInstance()
	{
		static DealAdminEvent instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		DealAdminEvent::getInstance()->handle(e);
	}

	static void AddEvent_Profile(EventQueue* eq, int cmd);
	static void AddEvent_AddExp(EventQueue* eq, int gid, int fd, int64 uid, int exp);
	static void AddEvent_AddCredit(EventQueue* eq, int gid, int fd, int64 uid, int gold);
	static void AddEvent_SetLevel(EventQueue* eq, int gid, int fd, int64 uid, int level);
	static void AddEvent_BanChat(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min);
	static void AddEvent_BanLogin(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min);
	static void AddEvent_Reload(EventQueue* eq, int gid, int fd);

private:
	void handle(Event* e);
	WorldEventHandler* eh_;
	log4cxx::LoggerPtr logger_;

protected:

};

#endif
