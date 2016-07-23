#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceObtainUniverseHandle.h"
#include "RseObtainUniverse.pb.h"

RceObtainUniverseHandle::RceObtainUniverseHandle()
:CBaseEvent()
{

}
RceObtainUniverseHandle::~RceObtainUniverseHandle()
{

}

void RceObtainUniverseHandle::handle(Event* e)
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

	RseObtainUniverse rse;
	pPlayer->FillUniverse(&rse);
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
}
