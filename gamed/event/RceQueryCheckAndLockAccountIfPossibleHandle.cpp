#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceQueryCheckAndLockAccountIfPossibleHandle.h"
#include "RseQueryCheckAndLockAccountIfPossible.pb.h"

RceQueryCheckAndLockAccountIfPossibleHandle::RceQueryCheckAndLockAccountIfPossibleHandle()
:CBaseEvent()
{

}
RceQueryCheckAndLockAccountIfPossibleHandle::~RceQueryCheckAndLockAccountIfPossibleHandle()
{

}

void RceQueryCheckAndLockAccountIfPossibleHandle::handle(Event* e)
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

void RceQueryCheckAndLockAccountIfPossibleHandle::handle_selfload(Event* e)
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

	RceQueryCheckAndLockAccountIfPossible* req = e->mutable_ce_rcequerycheckandlockaccountifpossible();
	if( !req )
	{
		return;
	}

	RseQueryCheckAndLockAccountIfPossible *pRsp = e->mutable_se_rsequerycheckandlockaccountifpossible();
	pRsp->set_lockapplied(req->applylock());
	pRsp->set_lockrequested(1);
	pRsp->set_locktype(0);
	pRsp->set_locksuccess(1);
	string text;
	pRsp->SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryCheckAndLockAccountIfPossible, text);
	return;

	DB_Planet *pDBPlanet = pPlayer->GetPlanet(pPlayer->GetCurPlanetId());
	if(!pDBPlanet){
		pRsp->set_locksuccess(0);
		pRsp->set_locktype(7);
		string text;
		pRsp->SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseQueryCheckAndLockAccountIfPossible, text);
		return;
	}
	/*for(int i = 0; i < req->hangarsunitsinfo_size(); i++){
		HangarsUnitsInfo *pMsgUnit = req->mutable_hangarsunitsinfo(i);
		if(pMsgUnit){
			for (int k = 0; k < pDBPlanet->hangars_size(); k++){
				DB_Hangar *pDBUnit = pDBPlanet->hangars(k);
				if(pDBUnit && pDBUnit->sid() == pMsgUnit->sid()){
					
				}
			}
		}
	}*/

	int64 TID;
	safe_atoll(req->targetaccountid(), TID);
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
		if(0 == req->applylock()){
			if(pTUser->Online()){
				pRsp->set_locksuccess(0);
				pRsp->set_locktype(1);
				string text;
				pRsp->SerializeToString(&text);
				eh_->sendDataToUser(pUser->fd(), S2C_RseQueryCheckAndLockAccountIfPossible, text);
				return;
			}else if(pTUser->GetLastAttackedTime()){
				pRsp->set_locksuccess(0);
				pRsp->set_locktype(2);
				string text;
				pRsp->SerializeToString(&text);
				eh_->sendDataToUser(pUser->fd(), S2C_RseQueryCheckAndLockAccountIfPossible, text);
				return;
			}
		}else{
			int nPlanetId = req->planetid();
			Player *pTPlayer = pUser->GetPlayer();
			
		}
	}
}

void RceQueryCheckAndLockAccountIfPossibleHandle::handle_romateload(Event* e)
{
	
}

void RceQueryCheckAndLockAccountIfPossibleHandle::handle_romatereturn(Event* e)
{
	
}
