#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceUpdateItemHandle.h"
#include "../../logic/SkuIdTbl.h"
#include "../../logic/ResourceSilos.h"

RceUpdateItemHandle::RceUpdateItemHandle()
:CBaseEvent()
{

}
RceUpdateItemHandle::~RceUpdateItemHandle()
{

}

void RceUpdateItemHandle::handle(Event* e)
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

	RceUpdateItem* req = e->mutable_ce_rceupdateitem();
	if( !req )
	{
		return;
	}

	RseUpdateItem rse;
	rse.set_ret(RseUpdateItemRet_Success);

	int nPlanetId = req->planetid();
	DB_Planet *pPlanet = pPlayer->GetPlanet(nPlanetId);
	if(!pPlanet){
		SendRet2User(pUser,RseUpdateItemRet_CantFindPlanet,rse);
		return;	
	}

	string strAction = req->action();
	MsgTransaction *pmt = NULL;
	if(req->has_transaction()){
		pmt = req->mutable_transaction();
	}
	
	if ("newItem" == strAction) 
	{
		if(pmt){
			if (!pPlayer->CostRes(RC_Cash, pmt->cash()) ||
				!pPlayer->CostRes(RC_Coin, pmt->coins()) ||
				!pPlayer->CostRes(RC_Mineral, pmt->minerals()) ||
				!pPlayer->CostRes(RC_Exp, pmt->exp()) ||
				!pPlayer->CostRes(RC_Score, pmt->score()) ||
				!pPlayer->CheckDroidInUse(pPlanet, pmt->droids()))
			{
				SendRet2User(pUser,RseUpdateItemRet_ResNotEnough,rse);
				return;	
			}
		}
		
		MsgBuildingItem *pMI = req->mutable_item();
		int sku = SkuIDTblInst::instance().GetSku(pMI->sku());
		int maxNum = ResourceSilosTblInst::instance().GetMaxNum(sku,pPlanet->hqlevel());//最大能造几个
		int count = 0;
		for(int i = 0;i<pPlanet->items_size();i++)
		{
			DB_Item *pItem = pPlanet->mutable_items(i);
			if(pMI->sku() == pItem->sku())
				count++;
		}
		if(count < maxNum)
		{
			INT64 id = pPlayer->CreateBuilding(nPlanetId, pMI);
			rse.set_id(id);
			//create之后更新建筑物数量
			if(!id){
				SendRet2User(pUser,RseUpdateItemRet_CreateFailure,rse);
				return;
			}
		}

		eh_->getDataHandler()->markUserDirty(pUser);
		rse.set_sid(req->sid());
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
	}
	else if ("destroy"== strAction) 
	{
		if(pmt){
			if (!pPlayer->CostRes(RC_Cash, pmt->cash()) ||
				!pPlayer->CostRes(RC_Coin, pmt->coins()) ||
				!pPlayer->CostRes(RC_Mineral, pmt->minerals()) ||
				!pPlayer->CostRes(RC_Exp, pmt->exp()) ||
				!pPlayer->CostRes(RC_Score, pmt->score()) ||
				!pPlayer->CheckDroidInUse(pPlanet, pmt->droids()))
			{
				SendRet2User(pUser,RseUpdateItemRet_ResNotEnough,rse);
				return;	
			}
		}

		if(!pPlayer->DestroyBuilding(nPlanetId, req->id(), req->sid())){
			SendRet2User(pUser,RseUpdateItemRet_DestroyFailure,rse);
			return;
		}

		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	}
	else if ("rotate"== strAction)
	{
		if(!pPlayer->RotateBuilding(nPlanetId, req->id(), req->sid(),req->x(),req->y(),req->flip())){
			SendRet2User(pUser,RseUpdateItemRet_RotateFailure,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	}
	else if ("move"== strAction) 
	{
		if(!pPlayer->MoveBuilding(nPlanetId, req->id(), req->sid(),req->x(),req->y())){
			SendRet2User(pUser,RseUpdateItemRet_MoveFailure,rse);
			return;
		}

		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	} 
	else if ("newState"== strAction)
	{
		if (!pPlayer->CostRes(RC_Cash, pmt->cash()) ||
			!pPlayer->CostRes(RC_Coin, pmt->coins()) ||
			!pPlayer->CostRes(RC_Mineral, pmt->minerals()) ||
			!pPlayer->CostRes(RC_Exp, pmt->exp()) ||
			!pPlayer->CostRes(RC_Score, pmt->score()))
		{
			SendRet2User(pUser,RseUpdateItemRet_ResNotEnough,rse);
			return;
		}
		int64 time = req->time();
		//int incomeToRestore = req->incomeToRestore()//不知道什么情况下有
		int newState = req->newstate();
		int oldState = req->oldstate();
		if(!pPlayer->updateNewState(nPlanetId,newState,oldState,req->id(),req->sid(),req->time()));
		{
			SendRet2User(pUser,RseUpdateItemRet_UpdateNewStateFailure,rse);
			return ;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	} 
	else if ("repairingStart"== strAction)
	{
		
	} 
	else if ("repairingCompleted"== strAction) 
	{

	}
	else if ("upgradePremium"== strAction) 
	{
		if (!pPlayer->CostRes(RC_Cash, pmt->cash()) ||
			!pPlayer->CostRes(RC_Coin, pmt->coins()) ||
			!pPlayer->CostRes(RC_Mineral, pmt->minerals()) ||
			!pPlayer->CostRes(RC_Exp, pmt->exp()) ||
			!pPlayer->CostRes(RC_Score, pmt->score()))
		{
			SendRet2User(pUser,RseUpdateItemRet_ResNotEnough,rse);
			return;	
		}
		
		if(!pPlayer->upgradePremium(nPlanetId, req->id(), req->sid())){
			SendRet2User(pUser,RseUpdateItemRet_CancelBuildFailure,rse);
			return;
		}

		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	}
	else if ("cancelBuild"== strAction) 
	{
		if(!pPlayer->CostRes(RC_Coin, pmt->coins()))
		{
			SendRet2User(pUser,RseUpdateItemRet_CancelBuildFailure,rse);
			return;
		}

		if(!pPlayer->cancelBuild(nPlanetId, req->id(), req->sid())){
			SendRet2User(pUser,RseUpdateItemRet_CancelBuildFailure,rse);
			return;
		}

		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
		return;
	}
	else if ("cancelUpgrade"== strAction) 
	{
		if(!pPlayer->CostRes(RC_Coin, pmt->coins()))
		{
			SendRet2User(pUser,RseUpdateItemRet_CancelUpgradeFailure,rse);
			return;
		}

		if(!pPlayer->cancelUpgrade(nPlanetId, req->id(), req->sid(), req->time())){
			SendRet2User(pUser,RseUpdateItemRet_CancelUpgradeFailure,rse);
			return;
		}
		SendRet2User(pUser,RseUpdateItemRet_Success,rse);
	} 
	else if ("instantRepairAll"== strAction)
	{

	}
}

void RceUpdateItemHandle::SendRet2User(User* pUser,int nRet,RseUpdateItem& rseUpdateItem)
{
	rseUpdateItem.set_ret(nRet);
	string text;
	rseUpdateItem.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateItem,text);
}