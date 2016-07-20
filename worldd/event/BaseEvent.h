#ifndef _WORLD_BASEEVENT_H_ 
#define _WORLD_BASEEVENT_H_ 

#pragma once
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"
class User;
//enum LoadStatus;

#define RegistWorldEvent(className) \
static void createInstance(WorldEventHandler* eh) \
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
	virtual void registHandler() = 0;
protected:
	WorldEventHandler* eh_;
	log4cxx::LoggerPtr logger_;
	static CBaseEvent* instance_;
};

#endif
