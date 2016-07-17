#include "UpdateWorkingStatus.h"
#include "../WorldNetHandler.h"
#include "../WorldDataHandler.h"

void UpdateWorkingStatus::addEvent(EventQueue* eq, int state, int gid) 
{
	Event* e=eq->allocateEvent();
	e->set_cmd(EVENT_UPDATE_WORKING_STATUS);
	e->set_time(-1);
	e->set_state(state);
	e->set_uid(-1);
	WorkingStatusData* req = e->mutable_workingstatus();
	req->set_gid(gid);
	req->set_finish(false);
	eq->safePushEvent(e);
}

void UpdateWorkingStatus::handle(Event* e)
{
	if(!e->has_workingstatus())
	{
		return;
	}

	if (e->state() == UpdateWorkingStatus_GW_Disconn) // lost gamed, event from local
	{
		handle_GW_Disconn(e);
	}
	else if (e->state() == UpdateWorkingStatus_GW_Conn) // gamed connected, event from local
	{
		handle_GW_Conn(e);
	}
	else if (e->state() == UpdateWorkingStatus_GW_Sync) // receive user sync data
	{
		handle_GW_Sync(e);
	}
	else
	{
		LOG4CXX_ERROR(logger_, "Invalid Event.\n"<<e->DebugString());
	}
}

void UpdateWorkingStatus::handle_GW_Disconn(Event* e)
{
	const WorkingStatusData& req = e->workingstatus();
	int gid = req.gid();
	//bool finish = req.finish();
	WorldDataHandler* dh = eh_->getDataHandler();
	dh->setWorkingStatus(gid, LOST_CONNECTION);
	LOG4CXX_INFO(logger_, "Switch world working status to CONNECTING.");
}

void UpdateWorkingStatus::handle_GW_Conn(Event* e)
{
	WorldDataHandler* dh = eh_->getDataHandler();
	const WorkingStatusData& req = e->workingstatus();
	int gid = req.gid();
	//bool finish = req.finish();
	dh->setWorkingStatus(gid, NOT_SYNC_USER);
	e->set_state(UpdateWorkingStatus_WG_Sync);
	LOG4CXX_INFO(logger_, "Request gamed #"<<gid<<" for sync.");
	eh_->sendEventToGamed(e, gid);
}

void UpdateWorkingStatus::handle_GW_Sync(Event* e)
{
	WorldDataHandler* dh = eh_->getDataHandler();
	const WorkingStatusData& req = e->workingstatus();
	int gid = req.gid();
	bool finish = req.finish();
	if (finish) // finish
	{
		dh->setWorkingStatus(gid, NORMAL);
		LOG4CXX_INFO(logger_, "Gamed #"<<gid<<" sync done.");
		e->set_state(UpdateWorkingStatus_WG_Fin);
		eh_->sendEventToGamed(e, gid);
	}
	else
	{
		LOG4CXX_INFO(logger_, "Gamed #"<<gid<<" sync failed.");
	}
}
