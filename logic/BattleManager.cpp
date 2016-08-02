#include "BattleManager.h"
#include "Player.h"

Battle::Battle()
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

	m_mDeployUnits.clear();
	m_mSpecialAttack.clear();
	m_mItemEnergys.clear();
	m_mBunkerUnits.clear();
}

BattleManager::BattleManager(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pBattle = NULL;
};

BattleManager::~BattleManager()
{
	if(m_pBattle){
		delete m_pBattle;
		m_pBattle = NULL;
	}
	m_pPlayer = NULL;
}

void BattleManager::NewBattle(int64 nTargetPlayer, int nPlanetId, time_t time)
{
	if(!m_pBattle){
		m_pBattle = new Battle();
	}
	m_pBattle->clear();
	m_pBattle->SetAttackUId(m_pPlayer->GetParent()->GetUid());
	m_pBattle->SetCurPlanetId(m_pPlayer->GetCurPlanetId());
	m_pBattle->SetTargetUId(nTargetPlayer);
	m_pBattle->SetTargetPlanetId(nPlanetId);
	m_pBattle->SetTime(time);
}
