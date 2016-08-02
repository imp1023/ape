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
	RceObtainUniverse *pReq = e->mutable_ce_rceobtainuniverse();
	string targetAccountId = pReq->targetaccountid();
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

	if(pReq->has_targetadvisorsku()){
		RseObtainUniverse rse;
		rse.set_npc(pReq->targetadvisorsku());
		string strNpc = pReq->targetadvisorsku();
		if(strNpc == "npc_A" || strNpc == "npc_B" ||strNpc == "npc_C" || strNpc == "npc_D"){
			int skuId = SkuIDTblInst::instance().GetSku(strNpc);
			DB_Player *pDBPlayer = pPlayer->GetDBPlayer();
			if(pDBPlayer){
				for(int i = 0; i < pDBPlayer->npcs_size(); i++){
					DB_NPC *pDBNpc = pDBPlayer->mutable_npcs(i);
					if(pDBNpc && skuId == pDBNpc->sku()){
						MsgPlanet *pMsgPlanet = rse.add_planets();
						if(pMsgPlanet){
							pMsgPlanet->set_hqlevel(pDBNpc->hqlevel());
						}
						for(int k = 0; k < pDBNpc->items_size(); k++){
							DB_Item* pDBItem = pDBNpc->mutable_items(k);
							MsgBuildingItem *pMsgItem = rse.add_items();
							if(pDBItem && pMsgItem){
								pMsgItem->set_id(k+1);
								pMsgItem->set_sid(pDBItem->sid());
								pMsgItem->set_type(pDBItem->type());
								pMsgItem->set_upgradeid(pDBItem->upgradeid());
								pMsgItem->set_x(pDBItem->x());
								pMsgItem->set_y(pDBItem->y());
								pMsgItem->set_sku(pDBItem->sku());
								pMsgItem->set_state(pDBItem->state());
								pMsgItem->set_isflipped(pDBItem->isflipped());
								pMsgItem->set_incometorestore(pDBItem->incometorestore());
								pMsgItem->set_energy(pDBItem->energy());
								pMsgItem->set_energypercent(pDBItem->energypercent());
								pMsgItem->set_time(pDBItem->time());
								pMsgItem->set_repairing(pDBItem->repairing());
							}
						}
					}
				}
			}
		}
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseObtainUniverse,text);
		return;
	}

	if(pReq->has_firstrequest() && pReq->firstrequest()){
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
				GWG_BattleInfo *pInfo = e->mutable_battleinfo();
				pInfo->set_accountid(uid);
				DB_Planet *pDBPlanet = pPlayer->GetPlanet(pPlayer->GetCurPlanetId());
				pInfo->set_starsku(pDBPlanet->star().sku());
				pInfo->set_name(pUser->GetPlatformId());
				pInfo->set_url("");
				for(int idx = 0; idx < pDBPlanet->units_size(); idx++){
					DB_GameUnit *pDBGameUnit = pDBPlanet->mutable_units(idx);
					GWG_GameUnit *pMsgGameUnit = pInfo->add_units();
					if(pDBGameUnit && pMsgGameUnit){
						pMsgGameUnit->set_sku(pDBGameUnit->sku());
						pMsgGameUnit->set_unlock(pDBGameUnit->unlock());
						pMsgGameUnit->set_upgradeid(pDBGameUnit->upgradeid());
						pMsgGameUnit->set_timeleft(pDBGameUnit->timeleft());
						pMsgGameUnit->set_updateat(pDBGameUnit->updateat());
					}
				}
				
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
				time_t nTime = time(NULL);
				pPlayer->NewBattle(TID, nPlanetId, nTime);
				pPlayer->FillBattleData(&rse);
				DB_Player *pDBTPlayer = pTUser->GetDBPlayer();
				DB_BattleReplay *pDBBattleReplay = pDBTPlayer->add_battlereplay();
				pDBBattleReplay->set_time(nTime);
				pDBBattleReplay->set_planetid(nPlanetId);
				DB_Planet *pDBPlanet = pPlayer->GetPlanet(pPlayer->GetCurPlanetId());
				pDBBattleReplay->set_starsku(pDBPlanet->star().sku());
				pDBBattleReplay->set_name(pUser->GetPlatformId());
				pDBBattleReplay->set_url("");
				for(int idx = 0; idx < pDBPlanet->units_size(); idx++){
					pDBBattleReplay->add_units()->CopyFrom(pDBPlanet->units(idx));
				}

				DB_Player *pTmpPlayer = pDBBattleReplay->mutable_copyuser()->mutable_player();
				pTmpPlayer->mutable_state()->CopyFrom(pTUser->GetDbUser().player().state());
				pTmpPlayer->mutable_flag()->CopyFrom(pTUser->GetDbUser().player().flag());

				//mission
				DB_Mission *pTmpMission = pTmpPlayer->mutable_missions();
				pTmpMission->set_readytostart(pDBTPlayer->missions().readytostart());
				pTmpMission->set_available(pDBTPlayer->missions().available());
				pTmpMission->set_completed(pDBTPlayer->missions().completed());
				pTmpMission->set_rewarded(pDBTPlayer->missions().rewarded());
				for(int i = 0; i < pDBTPlayer->missions().params_size(); i++){
					pTmpMission->add_params()->CopyFrom(pDBTPlayer->missions().params(i));
				}

				DB_Planet *pTmpDbPlanet = pTmpPlayer->add_planets();
				pTmpDbPlanet->mutable_star()->CopyFrom(pDBPlanet->star());
				pTmpDbPlanet->set_id(pDBPlanet->id());
				pTmpDbPlanet->set_type(pDBPlanet->type());
				pTmpDbPlanet->set_hqlevel(pDBPlanet->hqlevel());
				pTmpDbPlanet->set_droids(pDBPlanet->droids());
				pTmpDbPlanet->set_droidinuse(pDBPlanet->droidinuse());
				pTmpDbPlanet->set_capital(pDBPlanet->capital());
				pTmpDbPlanet->set_coinslimit(pDBPlanet->coinslimit());
				pTmpDbPlanet->set_minerallimit(pDBPlanet->minerallimit());

				//items
				for(int i = 0; i < pDBPlanet->items_size(); i++){
					pTmpDbPlanet->add_items()->CopyFrom(pDBPlanet->items(i));
				}
				//hangar
				for (int i = 0; i < pDBPlanet->hangars_size(); i++){
					DB_Hangar *pDBHangar = pDBPlanet->mutable_hangars(i);
					DB_Hangar *pMsgHangars = pTmpDbPlanet->add_hangars();
					if(pDBHangar && pMsgHangars){
						pMsgHangars->set_sid(pDBHangar->sid());
						for(int j = 0; j < pDBHangar->units_size(); j++){
							DB_HangarUnit *pHangarUnit = pDBHangar->mutable_units(j);
							DB_HangarUnit *pMsgHangarUnit = pMsgHangars->add_units();
							if(pHangarUnit && pMsgHangarUnit){
								pMsgHangarUnit->set_sku(pHangarUnit->sku());
								pMsgHangarUnit->set_num(pHangarUnit->num());
							}
						}
					}
				}
				//bunker
				for (int i = 0; i < pDBPlanet->bunkers_size(); i++){
					DB_Bunker *pDBBunker = pDBPlanet->mutable_bunkers(i);
					DB_Bunker *pMsgBunker = pTmpDbPlanet->add_bunkers();
					if(pDBBunker && pMsgBunker){
						pMsgBunker->set_sid(pDBBunker->sid());
						pMsgBunker->set_helpersaccountids(pDBBunker->helpersaccountids());
						for(int j = 0; j < pDBBunker->bunker_size(); j++){
							DB_BunkerUnit *pBunkerUnit = pDBBunker->mutable_bunker(j);
							for(int z = 0 ;z < pBunkerUnit->num();z++)
							{
								DB_BunkerUnit *pMsgBunkerUnit = pMsgBunker->add_bunker();
								if(pBunkerUnit && pMsgBunkerUnit){
									pMsgBunkerUnit->set_sku(pBunkerUnit->sku());
								}
							}
						}
					}
				}
				//gameunit
				for (int i = 0; i < pDBPlanet->units_size(); i++){
					DB_GameUnit *pDBUnit = pDBPlanet->mutable_units(i);
					DB_GameUnit *pMsgUnit = pTmpDbPlanet->add_units();
					if(pDBUnit && pMsgUnit){
						pMsgUnit->set_sku(pDBUnit->sku());
						pMsgUnit->set_unlock(pDBUnit->unlock());
						pMsgUnit->set_timeleft(pDBUnit->timeleft() - (time(NULL)-pDBUnit->updateat())*1000);//-1是没在升级
						pMsgUnit->set_upgradeid(pDBUnit->upgradeid());
						pMsgUnit->set_updateat(pDBUnit->updateat());
					}
				}
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
	if(pReq->attack()){
		DB_Player *pDBTPlayer = pTUser->GetDBPlayer();
		DB_Planet *pDBPlanet = pTUser->GetPlayer()->GetPlanet(pReq->planetid());
		DB_BattleReplay *pBattleReplay = pDBTPlayer->add_battlereplay();
		GWG_BattleInfo *pInf = e->mutable_battleinfo();
		time_t nTime = time(NULL);
		pBattleReplay->set_time(nTime);
		pBattleReplay->set_planetid(pReq->planetid());
		pBattleReplay->set_starsku(pInf->starsku());
		pBattleReplay->set_name(pInf->name());
		pBattleReplay->set_url(pInf->url());
		for(int idx = 0; idx < pInf->units_size(); idx++){
			GWG_GameUnit *pDBGameUnit = pInf->mutable_units(idx);
			DB_GameUnit *pMsgGameUnit = pBattleReplay->add_units();
			if(pDBGameUnit && pMsgGameUnit){
				pMsgGameUnit->set_sku(pDBGameUnit->sku());
				pMsgGameUnit->set_unlock(pDBGameUnit->unlock());
				pMsgGameUnit->set_upgradeid(pDBGameUnit->upgradeid());
				pMsgGameUnit->set_timeleft(pDBGameUnit->timeleft());
				pMsgGameUnit->set_updateat(pDBGameUnit->updateat());
			}
		}
		//pBattleReplay->mutable_copyuser()->CopyFrom(pTUser->GetDbUser());
		DB_Player *pTmpPlayer = pBattleReplay->mutable_copyuser()->mutable_player();
		pTmpPlayer->mutable_state()->CopyFrom(pTUser->GetDbUser().player().state());
		pTmpPlayer->mutable_flag()->CopyFrom(pTUser->GetDbUser().player().flag());

		//mission
		DB_Mission *pTmpMission = pTmpPlayer->mutable_missions();
		pTmpMission->set_readytostart(pDBTPlayer->missions().readytostart());
		pTmpMission->set_available(pDBTPlayer->missions().available());
		pTmpMission->set_completed(pDBTPlayer->missions().completed());
		pTmpMission->set_rewarded(pDBTPlayer->missions().rewarded());
		for(int i = 0; i < pDBTPlayer->missions().params_size(); i++){
			pTmpMission->add_params()->CopyFrom(pDBTPlayer->missions().params(i));
		}

		DB_Planet *pTmpDbPlanet = pTmpPlayer->add_planets();
		pTmpDbPlanet->mutable_star()->CopyFrom(pDBPlanet->star());
		pTmpDbPlanet->set_id(pDBPlanet->id());
		pTmpDbPlanet->set_type(pDBPlanet->type());
		pTmpDbPlanet->set_hqlevel(pDBPlanet->hqlevel());
		pTmpDbPlanet->set_droids(pDBPlanet->droids());
		pTmpDbPlanet->set_droidinuse(pDBPlanet->droidinuse());
		pTmpDbPlanet->set_capital(pDBPlanet->capital());
		pTmpDbPlanet->set_coinslimit(pDBPlanet->coinslimit());
		pTmpDbPlanet->set_minerallimit(pDBPlanet->minerallimit());

		//items
		for(int i = 0; i < pDBPlanet->items_size(); i++){
			pTmpDbPlanet->add_items()->CopyFrom(pDBPlanet->items(i));
		}
		//hangar
		for (int i = 0; i < pDBPlanet->hangars_size(); i++){
			DB_Hangar *pDBHangar = pDBPlanet->mutable_hangars(i);
			DB_Hangar *pMsgHangars = pTmpDbPlanet->add_hangars();
			if(pDBHangar && pMsgHangars){
				pMsgHangars->set_sid(pDBHangar->sid());
				for(int j = 0; j < pDBHangar->units_size(); j++){
					DB_HangarUnit *pHangarUnit = pDBHangar->mutable_units(j);
					DB_HangarUnit *pMsgHangarUnit = pMsgHangars->add_units();
					if(pHangarUnit && pMsgHangarUnit){
						pMsgHangarUnit->set_sku(pHangarUnit->sku());
						pMsgHangarUnit->set_num(pHangarUnit->num());
					}
				}
			}
		}
		//bunker
		for (int i = 0; i < pDBPlanet->bunkers_size(); i++){
			DB_Bunker *pDBBunker = pDBPlanet->mutable_bunkers(i);
			DB_Bunker *pMsgBunker = pTmpDbPlanet->add_bunkers();
			if(pDBBunker && pMsgBunker){
				pMsgBunker->set_sid(pDBBunker->sid());
				pMsgBunker->set_helpersaccountids(pDBBunker->helpersaccountids());
				for(int j = 0; j < pDBBunker->bunker_size(); j++){
					DB_BunkerUnit *pBunkerUnit = pDBBunker->mutable_bunker(j);
					for(int z = 0 ;z < pBunkerUnit->num();z++)
					{
						DB_BunkerUnit *pMsgBunkerUnit = pMsgBunker->add_bunker();
						if(pBunkerUnit && pMsgBunkerUnit){
							pMsgBunkerUnit->set_sku(pBunkerUnit->sku());
						}
					}
				}
			}
		}
		//gameunit
		for (int i = 0; i < pDBPlanet->units_size(); i++){
			DB_GameUnit *pDBUnit = pDBPlanet->mutable_units(i);
			DB_GameUnit *pMsgUnit = pTmpDbPlanet->add_units();
			if(pDBUnit && pMsgUnit){
				pMsgUnit->set_sku(pDBUnit->sku());
				pMsgUnit->set_unlock(pDBUnit->unlock());
				pMsgUnit->set_timeleft(pDBUnit->timeleft() - (time(NULL)-pDBUnit->updateat())*1000);//-1是没在升级
				pMsgUnit->set_upgradeid(pDBUnit->upgradeid());
				pMsgUnit->set_updateat(pDBUnit->updateat());
			}
		}



		e->mutable_battleinfo()->set_time(nTime);
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
		pPlayer->NewBattle(TID, pReq->planetid(), e->mutable_battleinfo()->time());
		pPlayer->FillBattleData(pRsp);
	}
}
