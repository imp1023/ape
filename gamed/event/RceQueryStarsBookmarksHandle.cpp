#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceQueryStarsBookmarksHandle.h"
#include "RseQueryStarsBookmarks.pb.h"

RceQueryStarsBookmarksHandle::RceQueryStarsBookmarksHandle()
:CBaseEvent()
{

}
RceQueryStarsBookmarksHandle::~RceQueryStarsBookmarksHandle()
{

}

void RceQueryStarsBookmarksHandle::handle(Event* e)
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

	RceQueryStarsBookmarks *req = e->mutable_ce_rcequerystarsbookmarks();
	if(!req){
		return;
	}

	RseQueryStarsBookmarks rse;
	pPlayer->FillinBookmarks(&rse);
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryStarsBookmarks, text);
}
