#include "DealWorldEvent.h"
#include "../WorldNetHandler.h"
#include "../WorldDataHandler.h"
#include "../../common/json-util.h"

extern int G_PKSrvD_ID;

DealWorldEvent::DealWorldEvent()
: CBaseEvent()
{

}

DealWorldEvent::~DealWorldEvent()
{

}

/**
 * 注册转发动作
 * 所有不需要world逻辑直接转发的包.都在这里注册
 */
void DealWorldEvent::registHandler()
{
	eh_->getEventHandler()->registHandler(EVENT_GWG_FRIEND_REQUEST, ((ProcessRoutine) DealWorldEvent::handle_));
	eh_->getEventHandler()->registHandler(EVETN_GWG_GAMESTAR_REQUEST, ((ProcessRoutine) DealWorldEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_UPDATE_FRIEND_CACHE, ((ProcessRoutine) DealWorldEvent::handle_));
}

void DealWorldEvent::handle(Event* e)
{
    switch (e->cmd())
    {
        case EVENT_GWG_FRIEND_REQUEST:
			{
				//processEventForwardByPlatId(e);
			}
			break;
		case EVENT_UPDATE_FRIEND_CACHE:
			{
				//processEventUpdateFriendCache(e);
			}
			break;
        default:
			{
				processEventForward(e);
			}
            break;
    }
}

//无脑的将包转发出去
void DealWorldEvent::processEventForward(Event* e)
{
    if (e->uid() <= 0)
        return;
    if (e->state() == (int) Status_Normal_To_World)
    {
        int64 uid = 0;//e->forwardinfo().uid();
        if (uid > 0)
        {
            processEventForwardByUid(e);
        }
        else
        {
            processEventForwardByPlatId(e);
        }
    }
    else if (e->state() == (int) Status_Normal_Back_World)
    {
        e->set_state(Status_Normal_Back_Game);
        eh_->sendEventToUser(e, e->uid());
    }
    else
    {

        LOG4CXX_ERROR(logger_, "Invalid Event.\n" << e->DebugString());
    }
}

void DealWorldEvent::processEventForwardByUid(Event* e)
{
    if (e->uid() <= 0)
        return;

    if (e->state() == (int) Status_Normal_To_World)
    {
        e->set_state(Status_Normal_Logic_Game);
        eh_->sendEventToUser(e, 0);//e->forwardinfo().uid());
    }
    else if (e->state() == (int) Status_Normal_Back_World)
    {
        e->set_state(Status_Normal_Back_Game);
        eh_->sendEventToUser(e, e->uid());
    }
    else
    {

        LOG4CXX_ERROR(logger_, "Invalid Event.\n" << e->DebugString());
    }
}

void DealWorldEvent::processEventForwardByPlatId(Event *e)
{

}