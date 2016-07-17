#include "../../logic/Player.h"
#include "DealRseBroadcast.h"
#include "../../event/event.pb.h"

DealRseBroadcast::DealRseBroadcast()
:CBaseEvent()
{

}

DealRseBroadcast::~DealRseBroadcast()
{

}

void DealRseBroadcast::handle(Event* e)
{
	int64 uid = e->uid();
	GameDataHandler* dh = eh_->getDataHandler();
	User* pUser = dh->getUser(uid);
	if (pUser == NULL) return;

	string text;
	RseBroadcast* data = e->mutable_se_rsebroadcast();
	data->SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseBroadcast, text);	
}
