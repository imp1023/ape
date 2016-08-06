#include "BattleManager.h"
#include "Player.h"

Battle::Battle(DB_BattleReplay *pbr):m_pbr(pbr)
{
	clear();
}

Battle::~Battle()
{
	clear();
};

void Battle::clear()
{
	m_UIdTarget = 0;
	m_nTargetPlanetId = 0;
	m_UIdAttack = 0;
	m_nCurrentPlanetId = 0;
	m_coinsTaken = 0;
	m_mineralTaken = 0;
	m_damage = 0;
	m_planetDamage = 0;
	m_revenge = 0;
	//sku
	m_time = 0;
	m_obtainDamageProtectionTime = 0;
	m_nBattleType = BATTLE_TYPE_UNKNOWN;
	m_mDeployUnits.clear();
	m_mSpecialAttack.clear();
	m_mItemEnergys.clear();
	m_mBunkerUnits.clear();
}

BattleManager::BattleManager(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pBattle = new Battle(pPlayer->GetDBPlayer()->mutable_battlereplay());
};

BattleManager::~BattleManager()
{
	if(m_pBattle){
		delete m_pBattle;
		m_pBattle = NULL;
	}
	m_pPlayer = NULL;
}

void BattleManager::BeginBattle(int64 nAttacker, int nFromPlanet, int64 nTargetPlayer, int nPlanetId, time_t time)
{
	if(!m_pBattle){
		return;
	}

	m_pBattle->clear();
	m_pBattle->SetAttackUId(nAttacker);
	m_pBattle->SetCurPlanetId(nFromPlanet);
	m_pBattle->SetTargetUId(nTargetPlayer);
	m_pBattle->SetTargetPlanetId(nPlanetId);
	m_pBattle->SetTime(time);
}

void BattleManager::SetBattleType(int nType)
{
	if(!m_pBattle){
		return;
	}
	m_pBattle->SetBattleType(nType);
}

void BattleManager::CopyUniverse()
{
	DB_Player *pDBTPlayer = m_pPlayer->GetDBPlayer();
	DB_BattleReplay *pDBBattleReplay = pDBTPlayer->mutable_battlereplay();
	DB_Player *pTmpPlayer = pDBBattleReplay->mutable_copyuser()->mutable_player();
	pTmpPlayer->Clear();
	pTmpPlayer->mutable_state()->CopyFrom(pDBTPlayer->state());
	pTmpPlayer->mutable_flag()->CopyFrom(pDBTPlayer->flag());
	pTmpPlayer->mutable_missions()->CopyFrom(pDBTPlayer->missions());
	Battle *pBattle = m_pPlayer->GetBattle();
	DB_Planet *pDBPlanet = m_pPlayer->GetPlanet(pBattle->GetTargetPlanetId());
	pTmpPlayer->add_planets()->CopyFrom(*pDBPlanet);

#if 0
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
#endif
}
