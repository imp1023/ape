#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceObtainCustomizerHandle.h"


RceObtainCustomizerHandle::RceObtainCustomizerHandle()
:CBaseEvent()
{

}
RceObtainCustomizerHandle::~RceObtainCustomizerHandle()
{

}

void RceObtainCustomizerHandle::handle(Event* e)
{
	int64 uid = e->uid();
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}
	User *pUser = pUserManager->getUser(uid);
	if ( !pUser)
	{
		return;
	}

	RseObtainCustomizer rse;
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainCustomizer,text);
}
