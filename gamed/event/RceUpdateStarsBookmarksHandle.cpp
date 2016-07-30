#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceUpdateStarsBookmarksHandle.h"
#include "RseUpdateStarsBookmarks.pb.h"

RceUpdateStarsBookmarksHandle::RceUpdateStarsBookmarksHandle()
:CBaseEvent()
{

}
RceUpdateStarsBookmarksHandle::~RceUpdateStarsBookmarksHandle()
{

}

void RceUpdateStarsBookmarksHandle::handle(Event* e)
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

	RceUpdateStarsBookmarks* req = e->mutable_ce_rceupdatestarsbookmarks();
	if( !req )
	{
		return;
	}

	RseUpdateStarsBookmarks rse;
	rse.set_ret(0);

	string strAction = req->action();
	if("addBookmark" == strAction){
		if (!pPlayer->AddStarBookmark(req->starid(), req->startype(), req->starname(), req->coordx(), req->coordy())){
			rse.set_ret(1);
		}
	}else if("removeBookmark" == strAction){
		if ( !pPlayer->DelStarBookmark(req->starid())){
			rse.set_ret(1);
		}
	}
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryStarsBookmarks, text);
}
