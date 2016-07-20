#ifndef _BASEEVENT_H_
#define _BASEEVENT_H_
#pragma once
#include "../../event/EventDefine.h"
#include "../GameEventHandler.h"
#include "../GameDataHandler.h"
#include "MessageDef.h"

#define RegistEvent(className,event_id) \
static void createInstance(GameEventHandler* eh) \
{ \
	className* instance_ = getInstance(); \
	instance_->eh_ = eh; \
	eh->getEventHandler()->registHandler(event_id,((ProcessRoutine)className::handle_));\
}\
static className* getInstance()\
{\
	static className ins;\
	return &ins;\
}\
static void handle_(Event* e)\
{\
	className::getInstance()->handle(e);\
}


#define RegistEvent_SelfDef(className) \
	static void createInstance(GameEventHandler* eh) \
{ \
	className* instance_ = getInstance(); \
	instance_->eh_ = eh; \
	instance_->registHandler();	\
}\
	static className* getInstance()\
{\
	static className ins;\
	return &ins;\
}\
	static void handle_(Event* e)\
{\
	className::getInstance()->handle(e);\
}

class CBaseEvent {
public:
	CBaseEvent()
	{
		logger_ = log4cxx::Logger::getLogger("EventHelper");
	}

	virtual ~CBaseEvent() {}	

protected:
	virtual void handle(Event* e) = 0;
protected:
	GameEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
	static CBaseEvent* instance_;
};

#endif
