#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/SkuIdTbl.h"
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

	//visit npc_A npc_B npc_C npc_D
	RceObtainUniverse *pReq = e->mutable_ce_rceobtainuniverse();
	if(pReq->has_targetadvisorsku()){
		RseObtainUniverse rse;
		rse.set_npc(pReq->targetadvisorsku());
		string strNpc = pReq->targetadvisorsku();
		int skuId = SkuIDTblInst::instance().GetSku(strNpc);
		pPlayer->FillinNpcs(&rse, skuId);
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
		return;
	}

	//load universe first time
	if(pReq->has_firstrequest() && pReq->firstrequest()){
		RseObtainUniverse rse;
		pPlayer->FillinUniverse(&rse, ID_CAPITAL_PLANET);
		pPlayer->SetCurPlaentId(ID_CAPITAL_PLANET);
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
		return;
	}

	//visit another or self universe
	string targetAccountId = pReq->targetaccountid();
	int64 TID = 0;
	safe_atoll(targetAccountId, TID);
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
			if(pReq->attack()){
				GWG_BattleInfo *pInfo = e->mutable_battleinfo();
				pPlayer->GetGWGBattleInfo(pInfo);
			}
			e->mutable_forwardinfo()->set_uid(TID);
			e->set_state(Status_Normal_To_World);
			eh_->sendEventToWorld(e);
		}
		return;
	}

	int nPlanetId = pReq->planetid();
	RseObtainUniverse rse;
	pTUser->GetPlayer()->FillinUniverse(&rse, nPlanetId);
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);

	if(uid == TID){
		pPlayer->SetCurPlaentId(nPlanetId);
	}

	if(pReq->attack()){
		time_t nTime = time(NULL);
		pPlayer->BeginBattle(uid, pPlayer->GetCurPlanetId(), TID, nPlanetId, nTime);
		pPlayer->SetBattleType(Battle::BATTLE_TYPE_PVP);
		pTUser->GetPlayer()->BeginBattle(uid, pPlayer->GetCurPlanetId(), TID, nPlanetId, nTime);
		GWG_BattleInfo info;
		pPlayer->GetGWGBattleInfo(&info);
		pTUser->GetPlayer()->FillBattleLogAttackerInfo(&info);
		pTUser->GetPlayer()->CopyUniverse();
		pUserManager->markUserDirty(pTUser);
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

	Player *pTPlayer = pTUser->GetPlayer();
	if(!pTPlayer || !pTPlayer->CanUse()){
		return;
	}
	pTPlayer->FillinUniverse(e->mutable_se_rseobtainuniverse(), pReq->planetid());
	if(pReq->attack()){
		GWG_BattleInfo *pInf = e->mutable_battleinfo();
		pTPlayer->BeginBattle(pInf->accountid(), pInf->planetid(), TID, pReq->planetid(), pInf->time());
		pTPlayer->SetBattleType(Battle::BATTLE_TYPE_PVP);
		pTPlayer->FillBattleLogAttackerInfo(pInf);
		pTPlayer->CopyUniverse();
		pUserManager->markUserDirty(pTUser);
	}
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
		pPlayer->BeginBattle(uid, pPlayer->GetCurPlanetId(), TID, pReq->planetid(), e->mutable_battleinfo()->time());
		pPlayer->SetBattleType(Battle::BATTLE_TYPE_PVP);
	}
}
