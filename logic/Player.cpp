#include <cmath>
#include "Player.h"
#include "dbinterface.pb.h"
#include "../common/SysLog.h"
#include "../gamed/GameDataHandler.h"
#include "../gamed/event/BaseEvent.h"
#include "../common/const_def.h"
#include "../common/Clock.h"
#include "../common/string-util.h"
#include "../common/rand_util.h"
#include "../common/DateTime.h"
#include "../event/RseBroadcast.pb.h"

//table
#include "CountryNameTbl.h"
#include "InitItemTbl.h"
#include "GameUnitsTbl.h"
#include "SkuIdTbl.h"
#include "NpcInitItemTbl.h"

//logic
#include "StarLogic.h"

#define DELETE_POBJ(pObj) \
{\
	if (pObj)\
	{\
		delete pObj;\
		pObj = NULL;\
	}\
}

Player::Player(User* pParent, DB_Player* pdbPlayer)
{
	m_pdbPlayer   			= pdbPlayer;
	m_pUser       			= pParent;
	eh_           			= NULL;
}

Player::~Player(void)
{
	//DELETE_POBJ(m_pBuildMgr);
}

void Player::SetEventHandler(GameEventHandler* eh)
{
    eh_ = eh;
}

void Player::SendBroadCastToSelf(int type, int id, vector<string> param)
{
	string text;
	RseBroadcast data;
	data.set_type(type);
	data.set_id(id);
	for(size_t i = 0; i < param.size(); i++)
		data.add_param(param[i]);
	data.SerializeToString(&text);
	if (m_pUser->Online())
		eh_->sendDataToUser(m_pUser->fd(), S2C_RseBroadcast, text);
}

void Player::InitPlayerData()
{
	InitPlayerModel();
	InitPlayerState();
	InitPlayerPlanets();
	InitPlayerNPC();
}

void Player::InitPlayerModel()
{
	m_pdbPlayer->mutable_model()->set_exp(0);
	m_pdbPlayer->mutable_model()->set_level(1);
	m_pdbPlayer->mutable_model()->set_coins(INIT_PLAYER_COINS);
	m_pdbPlayer->mutable_model()->set_minerals(INIT_PLAYER_MINERALS);
	m_pdbPlayer->mutable_model()->set_coinstotal(INIT_PLAYER_TOTALCOINS);
	m_pdbPlayer->mutable_model()->set_mineralstotal(INIT_PLAYER_TOTALMINERALS);
	m_pdbPlayer->mutable_model()->set_score(0);
	m_pdbPlayer->mutable_model()->set_exp(0);
	m_pdbPlayer->mutable_model()->set_cash(INIT_PLAYER_CASH);
}

void Player::InitPlayerState()
{
	m_pdbPlayer->mutable_state()->set_tutorialcompleted(0);
	m_pdbPlayer->mutable_state()->set_dmgprotectiontimeleft(time(NULL));
	m_pdbPlayer->mutable_state()->set_dmgprotectiontimetotal(INIT_PLAYER_DMGPROTECTIONTIMETOTAL);
}

void Player::InitPlayerPlanets()
{
	m_pdbPlayer->clear_planets();
	DB_Planet *pp = m_pdbPlayer->add_planets();
	if(!pp){return;}
	pp->set_id(1);
	pp->set_type(INIT_PLAYER_PLANETTYPE);
	pp->set_hqlevel(INIT_PLAYER_HQLEVEL);
	pp->set_droids(INIT_PLAYER_DROIDS);
	pp->set_droidinuse(0);
	pp->set_capital(1);
	pp->set_coinslimit(INIT_PLAYER_TOTALCOINS);
	pp->set_minerallimit(INIT_PLAYER_TOTALMINERALS);
	int nStarId = StarLogicInst::instance().GetNewPlayerStar(m_pUser->uid());
	pp->set_star(nStarId);

	//init planet item
	int num = InitItemTblInst::instance().GetTotalSize();
	for(int i = 1; i <= num; i++){
		CFG_ITEM *pCfgItem = InitItemTblInst::instance().GetInfo(i);
		if(pCfgItem){
			DB_Item *pDBItem = pp->add_items();
			if(pDBItem){
				pDBItem->set_sid(pCfgItem->sid);
				pDBItem->set_sku(pCfgItem->sku);
				pDBItem->set_type(pCfgItem->type);
				pDBItem->set_upgradeid(0);
				pDBItem->set_isflipped(0);
				pDBItem->set_energypercent(100);
				pDBItem->set_energy(pCfgItem->energy);
				pDBItem->set_x(pCfgItem->x);
				pDBItem->set_y(pCfgItem->y);
				pDBItem->set_state(1);
				pDBItem->set_time(0);
				pDBItem->set_incometorestore(0);
				pDBItem->set_repairing(0);
				pDBItem->set_repairstart(0);
				pDBItem->set_updateat(time(NULL));
			}
		}
	}

	//init hangar
	DB_Hangar *pHangar = pp->add_hangars();
	if(pHangar){
		pHangar->set_sid(40);
		pHangar->set_sku(0);
		pHangar->set_num(0);
	}

	//init game unit
	int nGameUnitNum = GameUnitTblInst::instance().GetTotalSize();
	for(int i = 1; i <= nGameUnitNum; i++){
		CFG_GameUnit *pCfgUnit = GameUnitTblInst::instance().GetInfo(i);
		if(pCfgUnit){
			DB_GameUnit *pDBUnit = pp->add_units();
			if(pDBUnit){
				pDBUnit->set_sku(pCfgUnit->sku);
				pDBUnit->set_unlock(pCfgUnit->unlock);
				pDBUnit->set_upgradeid(0);
				pDBUnit->set_timeleft(0);
				pDBUnit->set_updateat(time(NULL));
			}
		}
	}

	//item sum

}

void Player::InitPlayerNPC()
{
	//npc_A
	int sku = SkuIDTblInst::instance().GetSku("npc_A");
	DB_NPC *pDBNpc = m_pdbPlayer->add_npcs();
	if(pDBNpc){
		pDBNpc->set_sku(sku);
		pDBNpc->set_exp(1959982841);
		pDBNpc->set_hqlevel(6);
		pDBNpc->set_npcid(0);
		// no items
	}

	//npc_B
	sku = SkuIDTblInst::instance().GetSku("npc_B");
	pDBNpc = m_pdbPlayer->add_npcs();
	if(pDBNpc){
		pDBNpc->set_sku(sku);
		pDBNpc->set_exp(3910);
		pDBNpc->set_hqlevel(1);
		pDBNpc->set_npcid(0);
		vector<CFG_NPCInitItem*> vec;
		bool ret = NPCInitItemTblInst::instance().GetItems("npc_B", pDBNpc->npcid(), vec);
		if(ret){
			for(int i = 0; i < vec.size(); i++){
				CFG_NPCInitItem* pCfgItem = vec[i];
				DB_Item *pDBItem = pDBNpc->add_items();
				if(pDBItem){
					pDBItem->set_sid(pCfgItem->sid);
					pDBItem->set_sku(pCfgItem->sku);
					pDBItem->set_type(pCfgItem->type);
					pDBItem->set_upgradeid(pCfgItem->upgradeId);
					pDBItem->set_isflipped(pCfgItem->isFlipped);
					pDBItem->set_energy(pCfgItem->energy);
					pDBItem->set_energypercent(100);
					pDBItem->set_x(pCfgItem->x);
					pDBItem->set_y(pCfgItem->y);
					pDBItem->set_state(pCfgItem->state);
					pDBItem->set_time(pCfgItem->time);
					pDBItem->set_incometorestore(0);
					pDBItem->set_updateat(time(NULL));
				}
			}
		}
	}
	//npc_C
	sku = SkuIDTblInst::instance().GetSku("npc_C");
	pDBNpc = m_pdbPlayer->add_npcs();
	if(pDBNpc){
		pDBNpc->set_sku(sku);
		pDBNpc->set_exp(119999);
		pDBNpc->set_hqlevel(1);
		pDBNpc->set_npcid(0);

		vector<CFG_NPCInitItem*> vec;
		bool ret = NPCInitItemTblInst::instance().GetItems("npc_C", pDBNpc->npcid(), vec);
		if(ret){
			for(int i = 0; i < vec.size(); i++){
				CFG_NPCInitItem* pCfgItem = vec[i];
				DB_Item *pDBItem = pDBNpc->add_items();
				if(pDBItem){
					pDBItem->set_sid(pCfgItem->sid);
					pDBItem->set_sku(pCfgItem->sku);
					pDBItem->set_type(pCfgItem->type);
					pDBItem->set_upgradeid(pCfgItem->upgradeId);
					pDBItem->set_isflipped(pCfgItem->isFlipped);
					pDBItem->set_energy(pCfgItem->energy);
					pDBItem->set_energypercent(100);
					pDBItem->set_x(pCfgItem->x);
					pDBItem->set_y(pCfgItem->y);
					pDBItem->set_state(pCfgItem->state);
					pDBItem->set_time(pCfgItem->time);
					pDBItem->set_incometorestore(0);
					pDBItem->set_updateat(time(NULL));
				}
			}
		}
	}
	//npc_D
	sku = SkuIDTblInst::instance().GetSku("npc_D");
	pDBNpc = m_pdbPlayer->add_npcs();
	if(pDBNpc){
		pDBNpc->set_sku(sku);
		pDBNpc->set_exp(1498);
		pDBNpc->set_hqlevel(1);
		pDBNpc->set_npcid(0);

		vector<CFG_NPCInitItem*> vec;
		bool ret = NPCInitItemTblInst::instance().GetItems("npc_D", pDBNpc->npcid(), vec);
		if(ret){
			for(int i = 0; i < vec.size(); i++){
				CFG_NPCInitItem* pCfgItem = vec[i];
				DB_Item *pDBItem = pDBNpc->add_items();
				if(pDBItem){
					pDBItem->set_sid(pCfgItem->sid);
					pDBItem->set_sku(pCfgItem->sku);
					pDBItem->set_type(pCfgItem->type);
					pDBItem->set_upgradeid(pCfgItem->upgradeId);
					pDBItem->set_isflipped(pCfgItem->isFlipped);
					pDBItem->set_energy(pCfgItem->energy);
					pDBItem->set_energypercent(100);
					pDBItem->set_x(pCfgItem->x);
					pDBItem->set_y(pCfgItem->y);
					pDBItem->set_state(pCfgItem->state);
					pDBItem->set_time(pCfgItem->time);
					pDBItem->set_incometorestore(0);
					pDBItem->set_updateat(time(NULL));
				}
			}
		}
	}
}

bool Player::SetNewDayData()
{
	//todo：刷新数据,走客户端请求
	int nowDay = Clock::getLocalDay();
// 	if (m_pdbPlayer->dataday() >= nowDay)
// 		return false;
// 	m_pdbPlayer->set_dataday(nowDay);
// 	SYS_UserStat(m_pUser, LT_SetNewDay, "", "", "", "", "", "", "", "");

	return true;
}

void Player::SetNewWeekData()
{
    int64 last = m_pUser->lastLoginTime();
    if (last == 0)
    {
        return;
    }
    else
    {
        DateTime lastT(last);
        DateTime now;
        if (DateTime::isSameWeek(lastT,now))
        {
            return;
        }
    }
}