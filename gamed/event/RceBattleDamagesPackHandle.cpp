#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/SkuIdTbl.h"
#include "RceBattleDamagesPackHandle.h"
#include "RseBattleDamagesPack.pb.h"

RceBattleDamagesPackHandle::RceBattleDamagesPackHandle()
:CBaseEvent()
{

}
RceBattleDamagesPackHandle::~RceBattleDamagesPackHandle()
{

}

void RceBattleDamagesPackHandle::handle(Event* e)
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

	Battle* pBattle = pPlayer->GetBattle();
	if(!pBattle){
		return;
	}

	DB_Planet *pPlanet = pPlayer->GetPlanet(pBattle->GetCurrentPlanetId());
	if(!pPlanet){
		return;
	}

	RceBattleDamagesPack* req = e->mutable_ce_rcebattledamagespack();
	if( !req )
	{
		return;
	}

	for(int i = 0; i < req->deployunits_size(); i++){
		MsgDeployUnits *pMsgDeploy = req->mutable_deployunits(i);
		if(pMsgDeploy){
			DeployUnits unit;
			unit.sid = pMsgDeploy->hangarsid();
			unit.sku = pMsgDeploy->unitsskus();
			unit.x = pMsgDeploy->x();
			unit.y = pMsgDeploy->y();
			unit.millis = pMsgDeploy->millis();
			pBattle->AddDeployUnits(unit);
// 			int nHangarId = pMsgDeploy->hangarsid();
// 			DB_Hangar *pHangar = NULL;
// 			for(int k = 0; k < pPlanet->hangars_size(); k++){
// 				DB_Hangar *tmp = pPlanet->mutable_hangars(k);
// 				if(tmp && tmp->sid() == nHangarId){
// 					pHangar = tmp;
// 					break;
// 				}
// 			}
// 			if(pHangar){
// 				for (int j = 0; j < pHangar->units_size(); j++){
// 					DB_HangarUnit *pHangarUnit = pHangar->mutable_units(j);
// 					int nSkuId = SkuIDTblInst::instance().GetSku(unit.sku);
// 					if(pHangarUnit->sku() == nSkuId){
// 						pHangarUnit->set_num(pHangarUnit->num() - 1);
// 					}
// 				}
// 			}
		}
	}

	for (int i = 0; i < req->itemdamaged_size(); i++){
		MsgItemDamaged *pMsgItem = req->mutable_itemdamaged(i);
		if(pMsgItem){
			if(pMsgItem->destroyed()){
				MsgTransaction *pMsgTransaction = pMsgItem->mutable_transaction();
				MsgTransaction *pMsgTragetTrans = pMsgItem->mutable_transactiontarget();
				TransactionBattle source, target;
				if(pMsgTransaction){
					source.cash = pMsgTransaction->cash();
					source.droids = pMsgTransaction->droids();
					source.coins = pMsgTransaction->coins();
					source.minerals = pMsgTransaction->minerals();
					source.exp = pMsgTransaction->exp();
					source.score = pMsgTransaction->score();
				}
				if(pMsgTragetTrans){
					target.cash = pMsgTragetTrans->cash();
					target.droids = pMsgTragetTrans->droids();
					target.coins = pMsgTragetTrans->coins();
					target.minerals = pMsgTragetTrans->minerals();
					target.exp = pMsgTragetTrans->exp();
					target.score = pMsgTragetTrans->score();
				}
				pBattle->Transaction(source, target);

				ItemEnergy ie;
				ie.sid = pMsgItem->itemsid();
				ie.energy = 0;
				ie.energyPercent = 0;
				pBattle->AddItemDamaged(ie);
			}else{
				ItemEnergy ie;
				ie.sid = pMsgItem->itemsid();
				int nCurEnergy = pMsgItem->energybeforeshot() - pMsgItem->damage();
				ie.energy = nCurEnergy > 0 ? nCurEnergy : 0;
				//ie.energyPercent = 100;
				pBattle->AddItemDamaged(ie);
			}
		}
	}

	for(int i = 0; i < req->unitdamaged_size(); i++){
		MsgUnitDamaged *pMsgUnit = req->mutable_unitdamaged(i);
		if(pMsgUnit){
			if(pMsgUnit->destroyed() && -1 != pMsgUnit->frombunkersid()){
				BunkerUnit bu;
				bu.sid = pMsgUnit->frombunkersid();
				bu.sku = pMsgUnit->unitsku();
				bu.num = 1;
				pBattle->AddUnitDamaged(bu);
			}
		}
	}

	for(int i = 0; i < req->specialattack_size(); i++){
		MsgSpecialAttack *pMsgSpecial = req->mutable_specialattack(i);
		if(pMsgSpecial){
			MsgTransaction *pMsgTransaction = pMsgSpecial->mutable_transaction();
			TransactionBattle source, target;
			if(pMsgTransaction){
				source.cash = pMsgTransaction->cash();
				source.droids = pMsgTransaction->droids();
				source.coins = pMsgTransaction->coins();
				source.minerals = pMsgTransaction->minerals();
				source.exp = pMsgTransaction->exp();
				source.score = pMsgTransaction->score();
			}
			pBattle->Transaction(source, target);

			SpecialAttack sa;
			sa.sku = pMsgSpecial->socialitemsku();
			sa.x = pMsgSpecial->x();
			sa.y = pMsgSpecial->y();
			sa.millis = pMsgSpecial->millis();
			pBattle->AddSpecialAttack(sa);
		}
	}

	RseBattleDamagesPack rsp;
	rsp.set_ret(0);
	string text;
	rsp.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseBattleDamagesPack, text);
}