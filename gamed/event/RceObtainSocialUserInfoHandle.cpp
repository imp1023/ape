#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceObtainSocialUserInfoHandle.h"
#include "RseObtainSocialUserInfo.pb.h"

RceObtainSocialUserInfoHandle::RceObtainSocialUserInfoHandle()
:CBaseEvent()
{

}
RceObtainSocialUserInfoHandle::~RceObtainSocialUserInfoHandle()
{

}

void RceObtainSocialUserInfoHandle::handle(Event* e)
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

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer || !pPlayer->CanUse())
	{
		return ;
	}

	RseObtainSocialUserInfo rse;
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainSocialUserInfo,text);
}
