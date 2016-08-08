#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include ".././logic/Player.h"
#include "RceFinishedHandle.h"
#include "RseFinished.pb.h"

RceFinishedHandle::RceFinishedHandle()
:CBaseEvent()
{

}
RceFinishedHandle::~RceFinishedHandle()
{

}

void RceFinishedHandle::handle(Event* e)
{
	if (e->state() == Status_Normal_Game )
	{
		handle_selfload(e);
	}
	else if (e->state() == Status_Normal_Logic_Game)
	{
		handle_romateload(e);
	}
	else if (e->state() == Status_Normal_Back_Game)
	{
		handle_romatereturn(e);
	}
}

void RceFinishedHandle::handle_selfload(Event* e)
{
	if(!e){
		return;
	}

	RceFinished *pReq = e->mutable_ce_rcefinished();
	if(!pReq){
		return;
	}

	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}

	int64 uid = e->uid();
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

	Battle *pBattle = pPlayer->GetBattle();
	if(!pBattle){
		return;
	}

	pBattle->SetBattleType(Battle::BATTLE_TYPE_FINISHED);

	if(pBattle->GetBattleType() == Battle::BATTLE_TYPE_PVE_ATTACK ||
		pBattle->GetBattleType() == Battle::BATTLE_TYPE_PVE_DEFENSE ||
		pBattle->GetBattleType() == Battle::BATTLE_TYPE_NPC){
		RseFinished rsp;
		string text;
		rsp.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseFinished, text);
		return;
	}

	LoadStatus state = LOAD_INVALID;
	int64 TID = pBattle->GetTargetUId();
	User *pTUser = pUserManager->getUser(TID, &state, true);
	if (pTUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		else if (state == LOAD_MISS)
		{	
			e->mutable_forwardinfo()->set_uid(TID);
			e->set_state(Status_Normal_To_World);
			eh_->sendEventToWorld(e);
		}
		return;
	}
	else
	{
		pTUser->GetPlayer()->SetBattleType(Battle::BATTLE_TYPE_FINISHED);
		RseFinished rsp;
		string text;
		rsp.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseFinished, text);
	}
}

void RceFinishedHandle::handle_romateload(Event* e)
{
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}

	LoadStatus state = LOAD_INVALID;
	int64 TID = e->mutable_forwardinfo()->uid();
	User *pTUser = pUserManager->getUser(TID, &state, true);
	if (pTUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		return;
	}

	pTUser->GetPlayer()->SetBattleType(Battle::BATTLE_TYPE_FINISHED);
	e->set_state(Status_Normal_Back_World);
	eh_->sendEventToWorld(e);
}

void RceFinishedHandle::handle_romatereturn(Event* e)
{
	int64 uid = e->uid();
	GameDataHandler* pUserManager = eh_->getDataHandler();
	LoadStatus state = LOAD_INVALID;
	User *pUser = pUserManager->getUser(uid);
	if (pUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		return;
	}

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer || !pPlayer->CanUse())
	{
		return ;
	}

	RseFinished rsp;
	string text;
	rsp.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseFinished, text);
}
