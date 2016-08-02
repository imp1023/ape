#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/SkuIdTbl.h"
#include "RceObtainBattleReplayHandle.h"
#include "RseObtainBattleReplay.pb.h"

RceObtainBattleReplayHandle::RceObtainBattleReplayHandle()
:CBaseEvent()
{

}
RceObtainBattleReplayHandle::~RceObtainBattleReplayHandle()
{

}

void RceObtainBattleReplayHandle::handle(Event* e)
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

	DB_Player *pDBPlayer = pPlayer->GetDBPlayer();
	if(!pDBPlayer){
		return;
	}

	RseObtainBattleReplay rse;
	if(pDBPlayer->battlereplay_size() == 0){
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseObtainBattleReplay,text);
		return;
	}

	DB_BattleReplay *pDBBattleReplaye = pDBPlayer->mutable_battlereplay(0);
	if(!pDBBattleReplaye){
		return;
	}

	rse.set_planetid(pDBBattleReplaye->planetid());
	rse.set_accountid(pDBBattleReplaye->accountid());
	rse.set_name(pDBBattleReplaye->name());
	rse.set_url(pDBBattleReplaye->url());
	for(int i = 0; i < pDBBattleReplaye->deployunits_size(); i++){
		//rse.add_deploys()->CopyFrom(pDBBattleReplaye->deployunits(i));
		MsgDeployUnit *pMsgUnit = rse.add_deploys();
		DB_BattleDeployUnit *pDBUnit = pDBBattleReplaye->mutable_deployunits(i);
		if(pMsgUnit && pDBUnit){
			pMsgUnit->set_sku(pDBUnit->sku());
			pMsgUnit->set_x(pDBUnit->x());
			pMsgUnit->set_y(pDBUnit->y());
			pMsgUnit->set_millis(pDBUnit->millis());
		}
	}
	for(int i = 0; i < pDBBattleReplaye->units_size(); i++){
		DB_GameUnit *pDBGameUnit = pDBBattleReplaye->mutable_units(i);
		MsgGameUnit *pMsgGameUnit = rse.add_attackergameunits();
		if(pDBGameUnit && pMsgGameUnit){
			pMsgGameUnit->set_updatedat(pDBGameUnit->updateat());
			pMsgGameUnit->set_unlocked(pDBGameUnit->unlock());
			pMsgGameUnit->set_timeleft(pDBGameUnit->timeleft());
			string sku = SkuIDTblInst::instance().GetName(pDBGameUnit->sku());
			pMsgGameUnit->set_sku(sku);
			pMsgGameUnit->set_upgradeid(pDBGameUnit->upgradeid());
		}
	}
	//db_player
	BattleUniverse *pMsgBattleUniverse = rse.mutable_universe();
	if(pMsgBattleUniverse){
		DB_Player pTP = pDBBattleReplaye->copyuser().player();
		pMsgBattleUniverse->set_dcplayername(pDBBattleReplaye->copyuser().platform_id());
		pMsgBattleUniverse->set_dcworldname("world_name");
		pMsgBattleUniverse->set_dcplayerrank(1);
		pMsgBattleUniverse->set_dccoins(0);
		pMsgBattleUniverse->set_dcminerals(0);
		pMsgBattleUniverse->set_dccash(0);
		pMsgBattleUniverse->set_tutorialcompleted(pTP.state().tutorialcompleted());
		pMsgBattleUniverse->set_damageprotectiontimeleft(pTP.state().dmgprotectiontimeleft());
		pMsgBattleUniverse->set_damageprotectiontimetotal(pTP.state().dmgprotectiontimetotal());
		char flag[128];
		sprintf(flag, "quality:%d,music:%d,sound:%d,alliancesWelcomeId:%d,", 
			pTP.flag().quality(), 
			pTP.flag().music(), 
			pTP.flag().effect(), 
			pTP.flag().alliancewelcome());
		pMsgBattleUniverse->set_flags(flag);
		
		//mission
		MsgMissionState *pMission = pMsgBattleUniverse->mutable_missions();
		if(pMission){
			pMission->set_readytostart(pTP.missions().readytostart());
			pMission->set_available(pTP.missions().available());
			pMission->set_completed(pTP.missions().completed());
			pMission->set_rewarded(pTP.missions().rewarded());
			for(int i = 0; i < pTP.missions().params_size(); i++){
				MsgMissionParam *pMsgParam = pMission->add_params();
				DB_MissionParam *pDBParam = pTP.mutable_missions()->mutable_params(i);
				if(pMsgParam && pDBParam){
					pMsgParam->set_sku(pDBParam->sku());
					pMsgParam->set_endtime(pDBParam->endtime());
					string progress = pDBParam->sku() + ":" + pDBParam->target();
					pMsgParam->set_progress(progress);
				}
			}
		}
		//planet
		DB_Planet *pTPlanet = pTP.mutable_planets(0);
		MsgPlanet *PMsgPlanet = pMsgBattleUniverse->add_planets();
		PMsgPlanet->set_startype(pTPlanet->star().startype());
		PMsgPlanet->set_starid(pTPlanet->star().starid());
		PMsgPlanet->set_starname(pTPlanet->star().starname());
		PMsgPlanet->set_sku(pTPlanet->star().sku());
		PMsgPlanet->set_planetid(pTPlanet->id());
		PMsgPlanet->set_planettype(pTPlanet->type());
		PMsgPlanet->set_coinslimit(pTPlanet->coinslimit());
		PMsgPlanet->set_mineralslimit(pTPlanet->minerallimit());
		PMsgPlanet->set_hqlevel(pTPlanet->hqlevel());
		PMsgPlanet->set_capital(pTPlanet->capital());
		//items
		for(int i = 0; i < pTPlanet->items_size(); i++){
			DB_Item *pDBItem = pTPlanet->mutable_items(i);
			MsgBuildingItem *pMsgItem = pMsgBattleUniverse->add_items();
			if(pDBItem && pMsgItem){
				pMsgItem->set_id(pDBItem->id());
				pMsgItem->set_sid(pDBItem->sid());
				pMsgItem->set_type(pDBItem->type());
				pMsgItem->set_upgradeid(pDBItem->upgradeid());
				pMsgItem->set_y(pDBItem->y());
				pMsgItem->set_x(pDBItem->x());
				pMsgItem->set_state(pDBItem->state());
				pMsgItem->set_energy(pDBItem->energy());
				pMsgItem->set_energypercent(pDBItem->energypercent());
				pMsgItem->set_sku(pDBItem->sku());
				pMsgItem->set_incometorestore(pDBItem->incometorestore());
				pMsgItem->set_repairing(pDBItem->repairing());
				pMsgItem->set_isflipped(pDBItem->isflipped());
			}
		}
		//shipyard
		for (int i = 0; i < pTPlanet->shipyard_size(); i++){
			DB_Shipyard *pDBShipyard = pTPlanet->mutable_shipyard(i);
			if(pDBShipyard->slot_size()>0)
			{
				MsgShipyard *pMsgShipyard = pMsgBattleUniverse->add_shipyards();
				if(pDBShipyard && pMsgShipyard){
					pMsgShipyard->set_sid(pDBShipyard->sid());
					pMsgShipyard->set_updatedat(pDBShipyard->updatedat());
					pMsgShipyard->set_unlockedslots(pDBShipyard->unlockedslots());
					pMsgShipyard->set_block(0);
				}
			}
		}
		//hangar
		for (int i = 0; i < pTPlanet->hangars_size(); i++){
			DB_Hangar *pDBHangar = pTPlanet->mutable_hangars(i);
			MsgHangars *pMsgHangars = pMsgBattleUniverse->add_hangars();
			if(pDBHangar && pMsgHangars){
				pMsgHangars->set_sid(pDBHangar->sid());
				for(int j = 0; j < pDBHangar->units_size(); j++){
					DB_HangarUnit *pHangarUnit = pDBHangar->mutable_units(j);
					MsgHangarUnit *pMsgHangarUnit = pMsgHangars->add_hangar();
					if(pHangarUnit && pMsgHangarUnit){
						string sku = SkuIDTblInst::instance().GetName(pHangarUnit->sku());
						pMsgHangarUnit->set_sku(sku);
						pMsgHangarUnit->set_num(pHangarUnit->num());
					}
				}
			}
		}
		//bunker
		for (int i = 0; i < pTPlanet->bunkers_size(); i++){
			DB_Bunker *pDBBunker = pTPlanet->mutable_bunkers(i);
			MsgBunker *pMsgBunker = pMsgBattleUniverse->add_bunkers();
			if(pDBBunker && pMsgBunker){
				pMsgBunker->set_sid(pDBBunker->sid());
				pMsgBunker->set_helpersaccountids(pDBBunker->helpersaccountids());
				for(int j = 0; j < pDBBunker->bunker_size(); j++){
					DB_BunkerUnit *pBunkerUnit = pDBBunker->mutable_bunker(j);
					for(int z = 0 ;z < pBunkerUnit->num();z++)
					{
						MsgBunkerUnit *pMsgBunkerUnit = pMsgBunker->add_bunker();
						if(pBunkerUnit && pMsgBunkerUnit){
							pMsgBunkerUnit->set_sku(pBunkerUnit->sku());
						}
					}
				}
			}
		}
		//gameunit
		for (int i = 0; i < pTPlanet->units_size(); i++){
			DB_GameUnit *pDBUnit = pTPlanet->mutable_units(i);
			MsgGameUnit *pMsgUnit = pMsgBattleUniverse->add_gameunits();
			if(pDBUnit && pMsgUnit){
				string name = SkuIDTblInst::instance().GetName(pDBUnit->sku());
				pMsgUnit->set_sku(name);
				pMsgUnit->set_unlocked(pDBUnit->unlock());
				pMsgUnit->set_timeleft(pDBUnit->timeleft() - (time(NULL)-pDBUnit->updateat())*1000);//-1是没在升级
				pMsgUnit->set_upgradeid(pDBUnit->upgradeid());
				pMsgUnit->set_updatedat(pDBUnit->updateat());
			}
		}
	}

	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainBattleReplay,text);
}
