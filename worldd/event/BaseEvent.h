#ifndef _WORLD_BASEEVENT_H_ 
#define _WORLD_BASEEVENT_H_ 

#pragma once
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"

class CBaseEvent {
public:
    CBaseEvent()
    {
        logger_ = log4cxx::Logger::getLogger("EventHelper");
    }

    virtual ~CBaseEvent() {}	

protected:
    virtual void handle(Event* e) = 0;
    virtual void regist() = 0;
protected:
    WorldEventHandler* eh_;
    log4cxx::LoggerPtr logger_;
    static CBaseEvent* instance_;
};

#define InstanceCreator(className) \
static void createInstance(WorldEventHandler* eh) \
{ \
	className* instance_ = getInstance(); \
	instance_->eh_ = eh; \
	instance_->regist();	\
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

#define RegistEvent(className, event_id) \
    eh_->getEventHandler()->registHandler(event_id,((ProcessRoutine)className::handle_))

#define BIND_1_EVENT(className, event_id1)  \
    InstanceCreator(className)              \
    void regist() {                         \
    RegistEvent(className, event_id1);      \
}

#define BIND_2_EVENT(className, event_id1, event_id2)\
    InstanceCreator(className)              \
    void regist() {                         \
    RegistEvent(className, event_id1);      \
    RegistEvent(className, event_id2);      \
}

#define BIND_3_EVENT(className, event_id1, event_id2, event_id3)\
    InstanceCreator(className)              \
    void regist() {                         \
    RegistEvent(className, event_id1);      \
    RegistEvent(className, event_id2);      \
    RegistEvent(className, event_id3);      \
}

#define BIND_4_EVENT(className, event_id1, event_id2, event_id3, event_id4)\
    InstanceCreator(className)              \
    void regist() {                         \
    RegistEvent(className, event_id1);      \
    RegistEvent(className, event_id2);      \
    RegistEvent(className, event_id3);      \
    RegistEvent(className, event_id4);      \
}

#endif
