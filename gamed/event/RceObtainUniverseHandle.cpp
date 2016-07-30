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

void RceObtainUniverseHandle::handle_selfload(Event* e)
{
	if(!e){
		return;
	}
	RceObtainUniverse *pReq = e->mutable_ce_rceobtainuniverse();
	string targetAccountId = pReq->targetaccountid();
	bool firstRequest = pReq->firstrequest();
	int64 TID = 0;
	safe_atoll(targetAccountId, TID);

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

	if(firstRequest){
		RseObtainUniverse rse;
		pPlayer->FillinUniverse(&rse, 1);
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
	}else{
		int nPlanetId = pReq->planetid();
		int nAttack = pReq->attack();

		LoadStatus state = LOAD_INVALID;
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
			RseObtainUniverse rse;
			pTUser->GetPlayer()->FillinUniverse(&rse, nPlanetId);
			string text;
			rse.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
			
			if(nAttack){
				pPlayer->NewBattle(TID, nPlanetId);
				pPlayer->FillBattleData(&rse);
			}
		}
	}
}

void RceObtainUniverseHandle::handle_romateload(Event* e)
{
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}

	RceObtainUniverse *pReq = e->mutable_ce_rceobtainuniverse();
	LoadStatus state = LOAD_INVALID;
	int64 TID = 0;
	string targetAccountId = pReq->targetaccountid();
	safe_atoll(targetAccountId, TID);
	User *pTUser = pUserManager->getUser(TID, &state, true);
	if (pTUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		return;
	}

	pTUser->GetPlayer()->FillinUniverse(e->mutable_se_rseobtainuniverse(), pReq->planetid());
	e->set_state(Status_Normal_Back_World);
	eh_->sendEventToWorld(e);
}

void RceObtainUniverseHandle::handle_romatereturn(Event* e)
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

	RseObtainUniverse *pRsp = e->mutable_se_rseobtainuniverse();
	string text;
	pRsp->SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);

	RceObtainUniverse *pReq = e->mutable_ce_rceobtainuniverse();
	if(pReq && pReq->attack()){
		string targetAccountId = pReq->targetaccountid();
		int64 TID = 0;
		safe_atoll(targetAccountId, TID);
		pPlayer->NewBattle(TID, pReq->planetid());
		pPlayer->FillBattleData(pRsp);
	}
}
