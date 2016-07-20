#include "SendRemoteUser.h"
#include "../WorldNetHandler.h"
#include "../WorldDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../event/EventQueue.h"

void SendRemoteUser::handle(Event* e)
{
	if (e->state() == SendRemoteUser_GW_Req)
	{
		if(!e->has_sendremoteuser_req())
		{
			return;
		}
		handle_GW_Req(e);
	}
	else
	{
		LOG4CXX_ERROR(logger_, "Invalid Event.\n"<<e->DebugString());
	}
}

void SendRemoteUser::handle_GW_Req(Event* e)
{
	WorldDataHandler* dh = eh_->getDataHandler();
	//const SendRemoteUser_Req& req = e->sendremoteuser_req();
	int64 uid = e->uid();
	e->set_state(SendRemoteUser_WG_Req);
	int gid = dh->getGamedIdByUserId(uid);

	eh_->sendEventToGamed(e, gid);
}
