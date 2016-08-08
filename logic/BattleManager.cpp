#include "BattleManager.h"
#include "Player.h"

Battle::Battle():m_pbr(NULL)
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
	m_pBattle = new Battle();
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

int BattleManager::GetBattleType()
{
	if(!m_pBattle){
		return Battle::BATTLE_TYPE_UNKNOWN;
	}
	return m_pBattle->GetBattleType();
}

void BattleManager::CopyUniverse()
{
	DB_Player *pDBTPlayer = m_pPlayer->GetDBPlayer();
	DB_BattleReplay *pDBBattleReplay = pDBTPlayer->mutable_battlereplay();
	pDBBattleReplay->clear_deployunits();
	pDBBattleReplay->clear_copyuser();
	DB_Player *pTmpPlayer = pDBBattleReplay->mutable_copyuser()->mutable_player();
	pTmpPlayer->mutable_state()->CopyFrom(pDBTPlayer->state());
	pTmpPlayer->mutable_flag()->CopyFrom(pDBTPlayer->flag());
	pTmpPlayer->mutable_missions()->CopyFrom(pDBTPlayer->missions());
	Battle *pBattle = m_pPlayer->GetBattle();
	DB_Planet *pDBPlanet = m_pPlayer->GetPlanet(pBattle->GetTargetPlanetId());
	pTmpPlayer->add_planets()->CopyFrom(*pDBPlanet);
}
