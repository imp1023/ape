#define _CRT_SECURE_NO_WARNINGS
#include "WorldEventHandler.h"
#include "WorldNetHandler.h"
#include "WorldDataHandler.h"

extern int G_WorldD_ID;

WorldEventHandler::WorldEventHandler(EventQueue *eq, WorldDataHandler* dh, WorldNetHandler *nh, int nid)
: eq_(eq), dh_(dh), nh_(nh), nid_(nid)
{
    eh_ = new EventHandler(eq, dh, nh, nid);
    logger_ = log4cxx::Logger::getLogger("WorldEventHandler");
    initialEventProcessors();
}

WorldEventHandler::~WorldEventHandler(void)
{
}

void WorldEventHandler::start()
{
    eh_->start();
}

bool WorldEventHandler::sendEventToUser(Event *e, int64 uid)
{
    int gid = dh_->getGamedIdByUserId(uid);
    return nh_->sendEventToGamed(e, gid);
}

bool WorldEventHandler::sendEventToGamed(Event *e, int gid)
{
    return nh_->sendEventToGamed(e, gid);
}

bool WorldEventHandler::sendEventToAllGamed(Event *e)
{
    return nh_->sendEventToAllGamed(e);
}

void WorldEventHandler::sendFdString(int fd, const string &content)
{
    LOG4CXX_DEBUG(logger_, "Response to Web fd:" << fd << " with json: " << content);
    nh_->sendSizedFdString(fd, content.c_str());
}

#include "event/UpdateWorkingStatus.h"
#include "event/UserLogin.h"
#include "event/SendRemoteUser.h"
#include "event/DealAdminEvent.h"
#include "event/DealWorldEvent.h"

void WorldEventHandler::initialEventProcessors()
{
    // add Event Processors here
    UpdateWorkingStatus::createInstance(this);
    UserLogin::createInstance(this);
    SendRemoteUser::createInstance(this);
    DealAdminEvent::createInstance(this);
    DealWorldEvent::createInstance(this);
}
