#pragma once
#include "../../event/EventDefine.h"
#include "../GameEventHandler.h"
#include "../GameDataHandler.h"
class User;

class DealCountryHandle
{
public:

    DealCountryHandle()
    {
        logger_ = log4cxx::Logger::getLogger("EventHelper");
    }

    ~DealCountryHandle()
    {
    }

    static void createInstance(GameEventHandler* eh);

    static DealCountryHandle* getInstance()
    {
        return instance_;
    }

    static void handle_(Event* e)
    {
        DealCountryHandle::getInstance()->handle(e);
    }
private:
    void    handle(Event* e);
	void	HandleCountryLite(Event* e);
	void	HandleQueyrStarInfo(Event* e);
	void	HandleUpdateAlliances(Event *e);

private:
    GameEventHandler* eh_;
    log4cxx::LoggerPtr logger_;
    static DealCountryHandle* instance_;
} ;

