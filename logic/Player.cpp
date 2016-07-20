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

Player::Player(User* pParent)//:m_rPVEFightManager(this),m_rCountryArenaMgr(),m_rWorldArenaMgr(),m_rRegionArenaMgr(),m_rHeroArenaMgr()
{
#if 0
	m_bHasSendInfoToStar = false;
	m_pUser = pParent;

	m_ltRecoveryTime = 0;
	m_pdbPlayer = NULL;
	eh_ = NULL;
	m_emStat = US_CanAttack;
	m_lstStarIDLst.clear();
	//	m_mapDayAttUserCnt.clear();
	m_lstGetStarTime = 0;
	m_curHonorPerDay = 0;
	m_nInviteFrdID  = 0;
	m_bInviteFrdHasLogonToday = false;
	m_pNPCManager  = new NPCManager(this);
	m_pBaseManager = new BaseManager(0,this);
	m_pBattleManager=new BattleManager(0,this);
	m_pCountryManager=new CountryManager(this);
	m_pRegionCountryManager = new RegionCountryManager(this);
	m_pGveMgr = new GveManager(this);
	m_pResourceMgr = new ResourceManager(this);
	m_pWarGameMgr = new WarGameManager(this);
	m_pBossBtlMgr = new BossBtlManager(this);
	//m_pHomeManager = new HomeManager(this);
	//m_pStoreManager	= new StoreManager(this);
	m_rmapUserTryAttacked.clear();
	m_rmapUserTryLoad.clear();
	m_nInviteFrdCnt = 0;
	m_bGiftGain = false;
	m_emStarGroup	= Star_Group_A;
	m_mapServerStat.clear();
#endif
}

Player::~Player(void)
{
	//DELETE_POBJ(m_pBuildMgr);
}

void Player::SetEventHandler(GameEventHandler* eh)
{
    eh_ = eh;
}

void Player::InitDB(DB_Player* pDBPlaper)
{
	m_pdbPlayer = pDBPlaper;

#if 0
	//判断合服
	//合服标记位，如果该值不为零切与DB值不符，则进行合服操作
	int nCombineRegionFlag = serverConfig.getRegionResetFlag(m_pUser->GetUserRegion());
	if(nCombineRegionFlag>0&&m_pdbPlayer->uniteregionflag()!=nCombineRegionFlag)
	{//合服数据操作
		if(m_pdbPlayer->base_size()<=0)
		{//最少1个基地
			m_pdbPlayer->add_base();
		}
		DB_Bases* pDBBase = pDBPlaper->mutable_base(0);
		DB_Country* pDBCountry = pDBBase->mutable_countrydata();
		DB_Guild* pDBGuild	= pDBBase->mutable_guilddata();
		//国家：回首都，清血战，清国家官员
		if(pDBCountry->countryid()>0)
		{
			pDBCountry->set_cityid(CountryInfoCfg::Instance().GetCapital(pDBCountry->countryid()));
		}
		m_pUser->GetDbUser().clear_officer();
		pDBCountry->clear_btlrecord();
		//公会：清工会战
		if(pDBGuild->guildid()>0)
		{
			DB_GuildBtl* pDBGBtl = pDBGuild->mutable_guildbtlinfo();
			pDBGBtl->clear_signuptm();
			pDBGBtl->clear_honorembravecnt();
			pDBGBtl->clear_addatkpct();
			pDBGBtl->clear_addbloodpct();
		}
		//竞技场：数据全清
		DB_ArenaInfo* pArenaInfo = pDBBase->mutable_arenainfo();
		int nScore = pArenaInfo->nintegralscore();
		pDBBase->clear_arenainfo();
		if(nScore>0)
		{
			pArenaInfo = pDBBase->mutable_arenainfo();
			pArenaInfo->set_nintegralscore(nScore);//保留总积分
		}

		pDBBase->clear_worldarenainfo();
		pDBBase->clear_regionarenainfo();

		pDBBase->clear_resourcemsg();

		m_pdbPlayer->set_uniteregionflag(nCombineRegionFlag);

		SYS_LOG(GetUserID(),LT_UniteRegion,m_pUser->GetUserRegion(),0,nScore);
	}



	for(int i=m_pdbPlayer->res_size();i<RC_Cnt;i++)
	{
		m_pdbPlayer->add_res(0);
	}
	for(int i=m_pdbPlayer->resmax_size();i<RC_Cnt;i++)
	{
		m_pdbPlayer->add_resmax(0);
	}
	if(m_pdbPlayer->base_size()<=0)
	{//最少1个基地
		m_pdbPlayer->add_base();
	}
	OldDBInfo2New();
	InitBagItemDB(m_pdbPlayer->mutable_bag());
	m_pBaseManager->InitDB(m_pdbPlayer->mutable_base(0));
	m_pNPCManager->InitDB(pDBPlaper);
	m_pBattleManager->InitDB(m_pdbPlayer->mutable_base(0));
	m_pCountryManager->InitDB(m_pdbPlayer->mutable_base(0));
	m_rPVEFightManager.Init(m_pdbPlayer->mutable_base(0)->mutable_pvefightdata());
	m_rCountryArenaMgr.Init(this);
	m_rWorldArenaMgr.Init(this);
	m_rRegionArenaMgr.Init(this);
	m_rHeroArenaMgr.Init(this);
	m_pRegionCountryManager->InitDB(m_pdbPlayer->mutable_base(0));
	m_pGveMgr->InitDB(m_pdbPlayer->mutable_base(0));
	m_pResourceMgr->InitDB(m_pdbPlayer->mutable_base(0));
	m_pWarGameMgr->InitDB(m_pdbPlayer->mutable_base(0));
	m_pBossBtlMgr->InitDB(m_pdbPlayer->mutable_base(0));
	CalExtAirArmy();
	CalExtEarthArmy();
	CalExtAtkEarthArmy();
	m_rmapUserTryAttacked.clear();
	m_rmapUserTryLoad.clear();
	m_nInviteFrdCnt = 0;
	m_bGiftGain = false;
	m_mapServerStat.clear();

	UpdateCombatPowerCheck();

	ProofreadRentParts(false);   //验证租用配件是否过期
#endif
}

void Player::InitNewUserFromCfg()
{
	/*if(!CanUse())
		return;
	const InitUserTbl& userTbl = InitUserCfg::Instance().GetInitUserTbl();


	m_pdbPlayer->set_credits(userTbl.m_nCredits);
	m_pdbPlayer->set_level(1);

	m_pdbPlayer->set_res(RC_Metal,userTbl.m_nMetal);
	m_pdbPlayer->set_res(RC_Oil,userTbl.m_nOil);
	m_pdbPlayer->set_res(RC_Electric,userTbl.m_nElectric);
	m_pdbPlayer->set_res(RC_Human,userTbl.m_nHuman);

	m_pBaseManager->InitNewUBaseFromCfg();
	m_pNPCManager->InitNewWmstatusFromCfg();


	m_pdbPlayer->set_tutorialstat(1);

	SetState(US_NewProtected);*/
}

void Player::SendBroadCastToSelf(int type, int id, vector<string> param)
{
// 	string text;
// 	RseBroadcast data;
// 	data.set_type(type);
// 	data.set_id(id);
// 	for(size_t i = 0; i < param.size(); i++)
// 		data.add_param(param[i]);
// 	data.SerializeToString(&text);
// 	if (m_pUser->Online())
// 		eh_->sendDataToUser(m_pUser->fd(), S2C_RseBroadcast, text);
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
// 	m_pdbPlayer->mutable_model()->set_coins(INIT_PLAYER_COINS);
// 	m_pdbPlayer->mutable_model()->set_minerals(INIT_PLAYER_MINERALS);
// 	m_pdbPlayer->mutable_model()->set_coinstotal(INIT_PLAYER_TOTALCOINS);
// 	m_pdbPlayer->mutable_model()->set_mineralstotal(INIT_PLAYER_TOTALMINERALS);
	m_pdbPlayer->mutable_model()->set_score(0);
	m_pdbPlayer->mutable_model()->set_exp(0);
/*	m_pdbPlayer->mutable_model()->set_cash(INIT_PLAYER_CASH);*/
}

void Player::InitPlayerState()
{
	m_pdbPlayer->mutable_state()->set_tutorialcompleted(0);
	m_pdbPlayer->mutable_state()->set_dmgprotectiontimeleft(time(NULL));
/*	m_pdbPlayer->mutable_state()->set_dmgprotectiontimetotal(INIT_PLAYER_DMGPROTECTIONTIMETOTAL);*/
}

void Player::InitPlayerPlanets()
{
	m_pdbPlayer->clear_planets();
	DB_Planet *pp = m_pdbPlayer->add_planets();
	if(!pp){return;}
	pp->set_id(1);
// 	pp->set_type(INIT_PLAYER_PLANETTYPE);
// 	pp->set_hqlevel(INIT_PLAYER_HQLEVEL);
// 	pp->set_droids(INIT_PLAYER_DROIDS);
	pp->set_droidinuse(0);
	pp->set_capital(1);
// 	pp->set_coinslimit(INIT_PLAYER_TOTALCOINS);
// 	pp->set_minerallimit(INIT_PLAYER_TOTALMINERALS);
	int nStarId = StarLogicInst::instance().GetNewPlayerStar(m_pUser->GetUid());
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
