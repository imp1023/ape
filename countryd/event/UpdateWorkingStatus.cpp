#include "UpdateWorkingStatus.h"
#include "../CountryNetHandler.h"
#include "../CountryDataHandler.h"

void UpdateWorkingStatus::addEvent(EventQueue* eq, int state, int gid) {
	Event* e=eq->allocateEvent();
	e->set_cmd(EVENT_UPDATE_WORKING_STATUS);
	e->set_time(-2);
	e->set_state(state);
	WorkingStatus_Req* req = e->mutable_workingstatus_req();
	req->set_gid(gid);
	req->set_finish(false);
	eq->safePushEvent(e);
}

void UpdateWorkingStatus::handle(Event* e)
{
	if(!e->has_workingstatus_req())
	{
		return;
	}

	if (e->state() == UpdateWorkingStatus_GH_Disconn) // lost gamed, event from local
	{
		handle_GH_Disconn(e);
	}
	else if (e->state() == UpdateWorkingStatus_GH_Conn) // gamed connected, event from local
	{
		handle_GH_Conn(e);
	}
	else if (e->state() == UpdateWorkingStatus_GH_Sync) // receive user sync data
	{
		handle_GH_Sync(e);
	}
	else
	{
		LOG4CXX_ERROR(logger_, "Invalid Event.\n"<<e->DebugString());
	}
}

void UpdateWorkingStatus::handle_GH_Disconn(Event* e)
{
	const WorkingStatus_Req& req = e->workingstatus_req();
	int gid = req.gid();
	CountryDataHandler* dh = eh_->getDataHandler();
	dh->setWorkingStatus(gid, LOST_CONNECTION);
	LOG4CXX_INFO(logger_, "Switch world working status to CONNECTING.");
	//ResetGameUser(gid,false);
	//ResetGameUser(gid,true);
}

void UpdateWorkingStatus::handle_GH_Conn(Event* e)
{
	CountryDataHandler* dh = eh_->getDataHandler();
	const WorkingStatus_Req& req = e->workingstatus_req();
	int gid = req.gid();
	dh->setWorkingStatus(gid, NOT_SYNC_USER);
	e->set_state(UpdateWorkingStatus_HG_Sync);
	LOG4CXX_INFO(logger_, "Request gamed #"<<gid<<" for sync.");
	eh_->sendEventToGamed(e, gid);

	eh_->SendCountryLiteInfo(gid);
}

void UpdateWorkingStatus::handle_GH_Sync(Event* e)
{
	CountryDataHandler* dh = eh_->getDataHandler();
	const WorkingStatus_Req& req = e->workingstatus_req();
	int gid = req.gid();
	if (req.finish()) // finish
	{
		dh->setWorkingStatus(gid, NORMAL);
		LOG4CXX_INFO(logger_, "Gamed #"<<gid<<" sync done.");
		e->set_state(UpdateWorkingStatus_HG_Fin);
		eh_->sendEventToGamed(e, gid);
	}
	else
	{
		LOG4CXX_INFO(logger_, "Gamed #"<<gid<<" sync failed.");
	}
}
