#pragma once

#include <map>
#include <string>
#include "../common/const_def.h"
using namespace std;

class Player;
class DB_BattleReplay;

struct SpecialAttack
{
	string sku;
	int x;
	int y;
	int millis;
};

struct BunkerUnit
{
	int sid;
	string sku;
	int num;
};

struct ItemEnergy
{
	int sid;
	int energy;
	int energyPercent;
};

struct DeployUnits
{
	int sid;
	int millis;
	int x;
	int y;
	string sku;
};

struct TransactionBattle
{
	TransactionBattle():coins(0), minerals(0), droids(0), score(0), exp(0), cash(0){}
	int coins;
	int minerals;
	int droids;
	int score;
	int exp;
	int cash;
};

class Battle
{
public:
	Battle(DB_BattleReplay *pbr);
	~Battle();

	enum{
		BATTLE_TYPE_UNKNOWN,
		BATTLE_TYPE_PVP,
		BATTLE_TYPE_PVE,
		BATTLE_TYPE_NPC,
		BATTLE_TYPE_ALLIANCE,
		BATTLE_TYPE_MAX
	};

	void clear();
	inline void SetTargetUId(int64 TID){m_UIdTarget = TID;}
	inline void SetAttackUId(int64 AID){m_UIdAttack = AID;}
	inline void SetCurPlanetId(int nPlanetId){m_nCurrentPlanetId = nPlanetId;}
	inline void SetTargetPlanetId(int nPlanetId){m_nTargetPlanetId = nPlanetId;}
	inline void SetTime(int64 nTime){m_time = nTime;}

	inline int64 GetAttackUId(){return m_UIdAttack;}
	inline int64 GetTargetUId(){return m_UIdTarget;}
	inline int GetAttackerPlanetId(){return m_nCurrentPlanetId;}
	inline int GetTargetPlanetId(){return m_nTargetPlanetId;}
	inline int64 GetTime(){return m_time;}

	void AddDeployUnits(DeployUnits unit){m_mDeployUnits.push_back(unit);}
	void AddSpecialAttack(SpecialAttack sa){m_mSpecialAttack.push_back(sa);}
	void AddItemDamaged(ItemEnergy ie){
		map<int, ItemEnergy>::iterator iter = m_mItemEnergys.find(ie.sid);
		if(iter == m_mItemEnergys.end()){
			m_mItemEnergys.insert(make_pair(ie.sid, ie));
		}else{
			iter->second.energy = ie.energy;
			iter->second.energyPercent = ie.energyPercent;
		}
	}
	void AddUnitDamaged(BunkerUnit bu){
		map<int, DefBunkerUnit>::iterator iter = m_mBunkerUnits.find(bu.sid);
		if(iter == m_mBunkerUnits.end()){
			map<string, int> mUnits;
			mUnits.insert(make_pair(bu.sku, bu.num));
			m_mBunkerUnits.insert(make_pair(bu.sid, mUnits));
		}else{
			map<string, int>::iterator iter_ = iter->second.find(bu.sku);
			iter_->second += bu.num;
		}
	}
	void Transaction(TransactionBattle source, TransactionBattle target){
		m_TransationSource.coins += source.coins;
		m_TransationSource.minerals += source.minerals;
		m_TransationSource.droids += source.droids;
		m_TransationSource.score += source.score;
		m_TransationSource.exp += source.exp;
		m_TransationSource.cash += source.cash;

		m_TransactonTarget.coins += target.coins;
		m_TransactonTarget.minerals += target.minerals;
		m_TransactonTarget.droids += target.droids;
		m_TransactonTarget.score += target.score;
		m_TransactonTarget.exp += target.exp;
		m_TransactonTarget.cash += target.cash;
	}
	inline void SetBattleType(int nType){
		if(nType > BATTLE_TYPE_UNKNOWN && nType < BATTLE_TYPE_MAX){
			m_nBattleType = nType;
		}
	}
	inline int GetBattleType(){return m_nBattleType;}
	int GetSourceCoins(){return m_TransationSource.coins;}
	int GetSourceMinerals(){return m_TransationSource.minerals;}
	int GetTargetCoins(){return m_TransationSource.coins;}
	int GetTargetMinerals(){return m_TransationSource.minerals;}

private:
	int64 m_UIdTarget;
	int m_nTargetPlanetId;
	int64 m_UIdAttack;
	int m_nCurrentPlanetId;

	int m_coinsTaken;
	int m_mineralTaken;
	int m_damage;
	int m_planetDamage;
	int m_revenge;
	//sku
	int64 m_time;
	int m_obtainDamageProtectionTime;

	int m_nBattleType;

	TransactionBattle m_TransationSource;
	TransactionBattle m_TransactonTarget;
	vector<DeployUnits> m_mDeployUnits;
	vector<SpecialAttack> m_mSpecialAttack;
	map<int, ItemEnergy> m_mItemEnergys;
	typedef map<string, int> DefBunkerUnit;
	map<int, DefBunkerUnit > m_mBunkerUnits;

private:
	DB_BattleReplay* m_pbr;
};

class BattleManager
{
public:
	BattleManager(Player *pPlayer);
	~BattleManager();
	
	void BeginBattle(int64 nAttacker, int nFromPlanet, int64 nTargetPlayer, int nPlanetId, time_t time);
	Battle* GetBattle(){return m_pBattle;}
	void CopyUniverse();
	void SetBattleType(int nType);

private:
	Player *m_pPlayer;
	Battle *m_pBattle;
};
