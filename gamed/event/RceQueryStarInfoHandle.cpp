#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceQueryStarInfoHandle.h"

RceQueryStarInfoHandle::RceQueryStarInfoHandle()
:CBaseEvent()
{

}
RceQueryStarInfoHandle::~RceQueryStarInfoHandle()
{

}

void RceQueryStarInfoHandle::handle(Event* e)
{
	if(e == NULL)
	{
		return;
	}

	if(e->state() == Status_Normal_Game)
	{
		handle_c2country(e);
	}

	if(e->state() == Status_Country_To_Game)
	{
		handle_country2s(e);
	}
}

void RceQueryStarInfoHandle::handle_c2country(Event *e)
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

	RceQueryStarInfo *req = e->mutable_ce_rcequerystarinfo();
	if(!req){
		return;
	}

	e->set_state(Status_Game_To_Country);
	eh_->sendEventToCountry(e, pUser->GetUserRegion(true));
}

void RceQueryStarInfoHandle::handle_country2s(Event *e)
{
	int64 uid = e->uid();
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
	{
		return;
	}
	LoadStatus state = LOAD_INVALID;
	User *pUser = pUserManager->getUser(uid, &state, true);
	if (pUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		return;
	}

	RseQueryStarInfo *pRsp = e->mutable_se_rsequerystarinfo();
	e->set_state(Status_Game_To_Country);
	string text;
	pRsp->SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryStarInfo,text);
}

