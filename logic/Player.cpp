#include <cmath>
#include <ctime>
#include "Player.h"
#include "dbinterface.pb.h"
#include "../common/SysLog.h"
#include "../gamed/GameDataHandler.h"
#include "../gamed/event/BaseEvent.h"
#include "../common/const_def.h"
#include "../common/Clock.h"
#include "../common/string-util.h"
#include "../common/rand_util.h"
#include "ItemTbl.h"

//table
#include "CountryNameTbl.h"
#include "InitItemTbl.h"
#include "GameUnitsTbl.h"
#include "SkuIdTbl.h"
#include "NpcInitItemTbl.h"
#include "ResourceSilos.h"

//logic
#include "PlanetManager.h"

#define DELETE_POBJ(pObj) \
{\
	if (pObj)\
	{\
		delete pObj;\
		pObj = NULL;\
	}\
}
bool bAdd = false;
Player::Player(User* pParent)//:m_rPVEFightManager(this),m_rCountryArenaMgr(),m_rWorldArenaMgr(),m_rRegionArenaMgr(),m_rHeroArenaMgr()
{
	m_pUser = pParent;
	m_pdbPlayer = NULL;
	eh_ = NULL;
	m_pPlanetManager = new PlanetManager(this);
	m_pBattleManager = new BattleManager(this);

#if 0
	m_bHasSendInfoToStar = false;
	m_ltRecoveryTime = 0;
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
	SafeDelete(m_pPlanetManager);
	SafeDelete(m_pBattleManager);

	m_pdbPlayer = NULL;
	m_pUser = NULL;
}

void Player::SetEventHandler(GameEventHandler* eh)
{
    eh_ = eh;
}

void Player::InitDB(DB_Player* pDBPlaper)
{
	m_pdbPlayer = pDBPlaper;
	m_pPlanetManager->InitDB(pDBPlaper);

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

void Player::InitPlayerFlag()
{
	m_pdbPlayer->mutable_flag()->set_music(1);
	m_pdbPlayer->mutable_flag()->set_effect(1);
	m_pdbPlayer->mutable_flag()->set_quality(1);
	m_pdbPlayer->mutable_flag()->set_alliancewelcome(0);
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
	m_pdbPlayer->mutable_state()->set_tutorialcompleted(1);
	m_pdbPlayer->mutable_state()->set_dmgprotectiontimeleft(time(NULL));
	m_pdbPlayer->mutable_state()->set_dmgprotectiontimetotal(INIT_PLAYER_DMGPROTECTIONTIMETOTAL);
}

void Player::InitPlayerPlanets(int nID, int nName, int nType, string sku)
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
	DB_StarLite *pStar = pp->mutable_star();
	if(!pStar){return;}
	pStar->set_starid(nID);
	pStar->set_starname(nName);
	pStar->set_startype(nType);
	pStar->set_sku(sku);

	//init planet item
	int num = InitItemTblInst::instance().GetTotalSize();
	for(int i = 1; i <= num; i++){
		CFG_InitItem *pCfgItem = InitItemTblInst::instance().GetInfo(i);
		if(pCfgItem){
			DB_Item *pDBItem = pp->add_items();
			if(pDBItem){
				pDBItem->set_id(BUILDING_ID_START + i);
				pDBItem->set_sid(pCfgItem->sid);
				pDBItem->set_sku(SkuIDTblInst::instance().GetName(pCfgItem->sku));
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
				pDBUnit->set_timeleft(-1);
				pDBUnit->set_updateat(0);
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
					pDBItem->set_sku(SkuIDTblInst::instance().GetName(pCfgItem->sku));
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
					pDBItem->set_sku(SkuIDTblInst::instance().GetName(pCfgItem->sku));
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
					pDBItem->set_sku(SkuIDTblInst::instance().GetName(pCfgItem->sku));
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

bool Player::CostRes(UserSource emType,int nCount,bool bLog,UserSource_CostType eCosttype)
{
	if(emType < 0 || emType >= RC_Cnt)
		return false;

	int64 nOldCnt = 0;
	switch (emType)
	{
	case RC_Coin:
		nOldCnt = m_pdbPlayer->model().coins();
		break;
	case RC_Mineral:
		nOldCnt = m_pdbPlayer->model().minerals();
		break;
	case RC_Cash:
		nOldCnt = m_pdbPlayer->model().cash();
		break;
	case RC_Exp:
		nOldCnt = m_pdbPlayer->model().exp();
		break;
	case RC_Score:
		nOldCnt = m_pdbPlayer->model().score();
		break;
	}
	int64 nNewCnt = nOldCnt + nCount;
	if(nNewCnt >= 0){
		switch (emType)
		{
		case RC_Coin:
			m_pdbPlayer->mutable_model()->set_coins(nNewCnt);
			break;
		case RC_Mineral:
			m_pdbPlayer->mutable_model()->set_minerals(nNewCnt);
			break;
		case RC_Cash:
			m_pdbPlayer->mutable_model()->set_cash(nNewCnt);
			break;
		case RC_Exp:
			m_pdbPlayer->mutable_model()->set_exp(nNewCnt);
			break;
		case RC_Score:
			m_pdbPlayer->mutable_model()->set_score(nNewCnt);
			break;
		}

		//AddResStat(1,emType,nCount);

#if 0
		if(eCosttype != E_UserSource_CostType_TREATSTRATEGIC)
		{
			//if(bLog == true)
			//{
			//	SendCostResStat(emType,nCount,eCosttype);
			//}
			if(nCount>5000)
			{
				SYS_LOG(m_pUser->GetUid(),LT_CostRes,0,0,emType<<nCount<<nOldCnt<<nNewCnt);
			}			
		}
#endif
		eh_->getDataHandler()->markUserDirty(m_pUser);
		return true;
	}
	return false;
}

int Player::GetRes(UserSource emType)
{
	if(emType < 0 || emType >= RC_Cnt)
		return false;

	int64 nOldCnt = 0;
	switch (emType)
	{
	case RC_Coin:
		nOldCnt = m_pdbPlayer->model().coins();
		break;
	case RC_Mineral:
		nOldCnt = m_pdbPlayer->model().minerals();
		break;
	case RC_Cash:
		nOldCnt = m_pdbPlayer->model().cash();
		break;
	case RC_Exp:
		nOldCnt = m_pdbPlayer->model().exp();
		break;
	case RC_Score:
		nOldCnt = m_pdbPlayer->model().score();
		break;
	}
	return nOldCnt;
}

void Player::RemoveSocialItem(string sku)
{
	for(int i = 0;i<m_pdbPlayer->socialitems_size();i++)
	{
		DB_SocialItem* pDBTmp= m_pdbPlayer->mutable_socialitems(i);
		if(pDBTmp->sku() == sku)
		{
			DB_SocialItem* pDBWI_last = m_pdbPlayer->mutable_socialitems(m_pdbPlayer->socialitems_size() - 1);
			if(pDBWI_last && pDBWI_last != pDBTmp)
			{
				pDBWI_last->Swap(pDBTmp);
			}
			m_pdbPlayer->mutable_socialitems()->RemoveLast();
		}
	}
	eh_->getDataHandler()->markUserDirty(m_pUser);
}

bool Player::SetDroidInUse(DB_Planet *pPlanet, int cnt)
{
	if(!pPlanet || pPlanet->droids() < cnt){
		return false;
	}
	pPlanet->set_droidinuse(cnt);
	return true;
}

bool Player::CheckDroidInUse(DB_Planet *pPlanet, int cnt)
{
	if(!pPlanet){
		return false;
	}
	int nOldDroidInUse = pPlanet->droidinuse();
	
	if(nOldDroidInUse + cnt > pPlanet->droids()){
		return false;
	}
	//pPlanet->set_droidinuse(pPlanet->droidinuse() + 1);
	return true;
}

bool Player::CheckDroids(DB_Planet *pPlanet, int workingCnt)
{
	if(!pPlanet || workingCnt > pPlanet->droids()){
		return false;
	}
	return true;
}

bool Player::CostSocialItem(string sku, int cnt)
{
	DB_SocialItem *pDbSocialItem = NULL;
	for (int i = 0; i < m_pdbPlayer->socialitems_size(); i++){
		DB_SocialItem *pTmp = m_pdbPlayer->mutable_socialitems(i);
		if(pTmp && pTmp->sku() == sku){
			pDbSocialItem = pTmp;
			break;
		}
	}
	if(!pDbSocialItem || pDbSocialItem->amount() < cnt){
		return false;
	}
	pDbSocialItem->set_amount(pDbSocialItem->amount() - cnt);
	return true;
}

bool Player::AddSocialItem(string sku, int cnt)
{
	DB_SocialItem *pDbSocialItem = NULL;
	for (int i = 0; i < m_pdbPlayer->socialitems_size(); i++){
		DB_SocialItem *pTmp = m_pdbPlayer->mutable_socialitems(i);
		if(pTmp && pTmp->sku() == sku){
			pDbSocialItem = pTmp;
			break;
		}
	}
	if(!pDbSocialItem){
		pDbSocialItem = m_pdbPlayer->add_socialitems();
		pDbSocialItem->set_sku(sku);
	}
	pDbSocialItem->set_amount(pDbSocialItem->amount() + cnt);
	return true;
}

DB_SocialItem* Player::GetSocialItem(string sku)
{
	DB_SocialItem *pDbSocialItem = NULL;
	for (int i = 0; i < m_pdbPlayer->socialitems_size(); i++){
		DB_SocialItem *pTmp = m_pdbPlayer->mutable_socialitems(i);
		if(pTmp && pTmp->sku() == sku){
			return pTmp;
		}
	}
	return NULL;
}

DB_Item * Player::CreateBuilding(int nPlanetId, MsgBuildingItem *pItem)
{
	if(!pItem){
		return 0;
	}
	
	DB_Item *pDBItem = m_pPlanetManager->CreateItem(nPlanetId, pItem);
	if(pDBItem){
		return pDBItem;
	}
	return NULL;
}

int Player::CreateBuilding(int nPlanetId, CFG_InitItem *pCfgItem)
{
	if(!pCfgItem){
		return 0;
	}
	
	DB_Item *pDBItem = m_pPlanetManager->CreateItem(nPlanetId, pCfgItem);
	if(pDBItem){
		return pDBItem->id();
	}
	return 0;
}

DB_Planet* Player::GetPlanet(int nPlanetId)
{
	if(!CanUse() || 0 >= nPlanetId){
		return NULL;
	}
	
	Planet *pPlanet = m_pPlanetManager->GetPlanet(nPlanetId);
	if(!pPlanet){
		return NULL;
	}
	return pPlanet->GetDBPlanet();
}

bool Player::AddStarBookmark(int nStarId, int nStarType, int nStarName, int x, int y)
{
	if(!CanUse() || 0 >= nStarId){
		return false;
	}

	bool bFind = false;
	for (int i = 0; i < m_pdbPlayer->bookmarks_size(); i++)
	{
		DB_StarsBookmark* pDBBookmark = m_pdbPlayer->mutable_bookmarks(i);
		if(pDBBookmark && pDBBookmark->starid() == nStarId){
			bFind = true;
		}
	}
	if(bFind){
		return false;
	}
	DB_StarsBookmark *pBookmark = m_pdbPlayer->add_bookmarks();
	if(!pBookmark){
		return false;
	}

	pBookmark->set_starname(nStarName);
	pBookmark->set_starid(nStarId);
	pBookmark->set_startype(nStarType);
	char sku[128] = {0};
	sprintf(sku, "%d:%d", x, y);
	pBookmark->set_sku(sku);
}

bool Player::DelStarBookmark(int nStarId)
{
	if(!CanUse() || 0 >= nStarId){
		return false;
	}

	for (int i = 0; i < m_pdbPlayer->bookmarks_size(); i++){
		DB_StarsBookmark* pDBBookmark = m_pdbPlayer->mutable_bookmarks(i);
		if(pDBBookmark && pDBBookmark->starid() == nStarId){
			DB_StarsBookmark* pLast = m_pdbPlayer->mutable_bookmarks(m_pdbPlayer->bookmarks_size() - 1);
			pLast->Swap(pDBBookmark);
			m_pdbPlayer->mutable_bookmarks()->RemoveLast();
			return true;
		}
	}
	return false;
}

bool Player::DestroyBuilding(int nPlanetId, int id, int sid)
{
	if(!CanUse()){
		return false;
	}
	DB_Item *pItem = m_pPlanetManager->GetItem(nPlanetId, id);
	if(!pItem){
		return false;
	}
	int nType = pItem->type();
	string sku = pItem->sku();
	bool ret = m_pPlanetManager->DestroyItem(nPlanetId, id);
	if(!ret){
		return false;
	}
	DB_Planet* pPlanet = GetPlanet(nPlanetId);
	switch(nType)
	{
	case 3://清除兵营
		for(int i = 0;i<pPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == sid)
			{
				DB_Shipyard* pDBShip_last = pPlanet->mutable_shipyard(pPlanet->shipyard_size() - 1);
				if(pDBShip_last && pDBShip_last != pDBShipyard)
				{
					pDBShip_last->Swap(pDBShipyard);
				}
				pPlanet->mutable_shipyard()->RemoveLast();
			}
		}
		break;
	case 7://清除传送门
		for(int i = 0;i<pPlanet->hangars_size();i++)
		{
			DB_Hangar* pDBHangar = pPlanet->mutable_hangars(i);
			if(pDBHangar->sid() == sid)
			{
				DB_Hangar* pDBHangar_last = pPlanet->mutable_hangars(pPlanet->hangars_size() - 1);
				if(pDBHangar_last && pDBHangar_last != pDBHangar)
				{
					pDBHangar_last->Swap(pDBHangar);
				}
				pPlanet->mutable_hangars()->RemoveLast();
			}
		}
		break;
	case 8://地堡
		for(int i = 0;i<pPlanet->bunkers_size();i++)
		{
			DB_Bunker* pDBBunker = pPlanet->mutable_bunkers(i);
			if(pDBBunker->sid() == sid)
			{
				DB_Bunker* pDBBunker_last = pPlanet->mutable_bunkers(pPlanet->bunkers_size() - 1);
				if(pDBBunker_last && pDBBunker_last != pDBBunker)
				{
					pDBBunker_last->Swap(pDBBunker);
				}
				pPlanet->mutable_bunkers()->RemoveLast();
			}
		}
		break;
	case 9://实验室
		break;
	default:
		break;
	}

	if(pItem->sku() == "rs_002_001")//银行
	{
		int lv = pItem->upgradeid() + 1;
		CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
		int slot = Tbl->slotCapacity;
		if(!updateCoinsLimit(-slot))
			return false;
	}
	if(pItem->sku() == "rs_001_001")//矿场
	{
		int lv = pItem->upgradeid() + 1;
		CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
		int slot = Tbl->slotCapacity;
		if(!updateMineralsLimit(-slot))
			return false;
	}
	return true;
}

bool Player::RotateBuilding(int nPlanetId, int id, int sid, int x, int y, int flip)
{
	DB_Item *pDBItem = m_pPlanetManager->GetItem(nPlanetId, id);
	if(!pDBItem){
		return false;
	}
	pDBItem->set_x(x);
	pDBItem->set_y(y);
	pDBItem->set_isflipped(flip);
	return true;
}

bool Player::MoveBuilding(int nPlanetId, int id, int sid, int x, int y)
{
	DB_Item *pDBItem = m_pPlanetManager->GetItem(nPlanetId, id);
	
	if(!pDBItem)
	{
		DB_Planet* pDBPlanet = GetPlanet(nPlanetId);
		for(int i = 0; i< pDBPlanet->items_size();i++)
		{
			DB_Item* pDBItem_sid= pDBPlanet->mutable_items(i);
			if(pDBItem_sid->sid() == sid)
			{
				pDBItem = pDBItem_sid;
				break;
			}
		}

	}
	if(!pDBItem){
		return false;
	}
	pDBItem->set_x(x);
	pDBItem->set_y(y);
	return true;
}

bool Player::cancelBuild(int nPlanetId, int id, int sid)
{
	DB_Item *pDBItem = m_pPlanetManager->GetItem(nPlanetId, id);
	if(!pDBItem)
		return false;

	pDBItem->Clear();
	return true;

}

bool Player::cancelUpgrade(int nPlanetId, int id, int sid, int ntime)
{

	DB_Item *pDBItem = m_pPlanetManager->GetItem(nPlanetId, id);
	if(!pDBItem){
		return false;
	}
	pDBItem->set_state(1);
	pDBItem->set_updateat(0);
	pDBItem->set_time(ntime);
	return true;
}

bool Player::upgradePremium(int nPlanetId, int id, int sid, int ntime , int incometorestore)
{
	Planet *pPlanet = m_pPlanetManager->GetPlanet(nPlanetId);
	
	if(!pPlanet){
		return false;
	}
	DB_Item *pDBItem = pPlanet->GetItem(id);
	if(!pDBItem)
	{
		DB_Planet* pDBPlanet = GetPlanet(nPlanetId);
		for(int i = 0; i< pDBPlanet->items_size();i++)
		{
			DB_Item* pDBItem_sid= pDBPlanet->mutable_items(i);
			if(pDBItem_sid->sid() == sid)
			{
				pDBItem = pDBItem_sid;
				break;
			}
		}
	}
	if(!pDBItem)
		return false;
	int sku = SkuIDTblInst::instance().GetSku(pDBItem->sku());
	if(!pDBItem){
		return false;
	}
	pDBItem->set_state(1);
	pDBItem->set_time(ntime);
	pDBItem->set_upgradeid(pDBItem->upgradeid() + 1);
	if (pDBItem->sku() == "rs_002_001") {// 银行
		int lv = pDBItem->upgradeid() + 1;
		CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
		int slot = Tbl->slotCapacity;
		//修改 上限
		if(!updateCoinsLimit(slot))
			return false;
	}
	if (pDBItem->sku() == "rs_001_001") {// 筒仓
		int lv = pDBItem->upgradeid() + 1;
		CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
		int slot = Tbl->slotCapacity;
		if(!updateMineralsLimit(slot))
			return false;
	}
	if (pDBItem->sku() == "wonders_headquarters") {// 主基地
		pPlanet->SetHQlevel(pPlanet->GetHQLevel() + 1);
	}
	int level = pDBItem->upgradeid() + 1;
	int energy = ResourceSilosTblInst::instance().GetEnergy(sku, level);
	pDBItem->set_energy(energy);
	pDBItem->set_incometorestore(incometorestore);

	return true;
}

bool Player::updateCoinsLimit(int slot)
{
	DB_Model * model = m_pdbPlayer->mutable_model();
	if(!model)
		return false;
	model->set_coinstotal(model->coinstotal() + slot);
	return true;
}

bool Player::updateMineralsLimit(int slot)
{
	DB_Model * model = m_pdbPlayer->mutable_model();
	if(!model)
		return false;
	model->set_mineralstotal(model->mineralstotal() + slot);
	return true;
}

bool Player::updateNewState(int nPlanetId,int newState, int oldState,int id, int sid, int ntime, int incomeToRestore)
{
	Planet *pPlanet = m_pPlanetManager->GetPlanet(nPlanetId);
	DB_Planet* pDBPlanet = pPlanet->GetDBPlanet();
	DB_Item *pDBItem = pPlanet->GetItem(id);
	if(!pDBItem)
	{
		for(int i = 0; i< pDBPlanet->items_size();i++)
		{
			DB_Item* pDBItem_sid= pDBPlanet->mutable_items(i);
			if(pDBItem_sid->sid() == sid)
			{
				pDBItem = pDBItem_sid;
			}
		}
		
	}
	if(!pDBItem)
		return false;
	int sku = SkuIDTblInst::instance().GetSku(pDBItem->sku());
	if(newState == 1 && oldState == 0)//建筑完成
	{
		//释放工人
		pDBPlanet->set_droidinuse(pDBPlanet->droidinuse() - 1);

		// 根据ItemType初始兵营、兵厂、实验室

		if(pDBPlanet->type() == 3)//shipyard
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->add_shipyard();
			pDBShipyard->set_sid(sid);
			pDBShipyard->set_unlockedslots(0);
			pDBShipyard->add_slot();
			pDBShipyard->add_slot();
			pDBShipyard->add_slot();
			//slot空出
		}

		if(pDBItem->type() == 7)
		{
			DB_Hangar* pDBHangar = pDBPlanet->add_hangars();
			pDBHangar->set_sid(sid);
		}

		if(pDBItem->type() == 8)
		{
			DB_Bunker* pDBBunker = pDBPlanet->add_bunkers();
			pDBBunker->set_sid(sid);
		}
		
		if (pDBItem->sku() == "rs_002_001" ) {// 银行
			int lv = pDBItem->upgradeid() + 1;
			CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
			int slot = Tbl->slotCapacity;
			//修改 上限
			if(!updateCoinsLimit(slot))
				return false;
		}
		if (pDBItem->sku() == "rs_001_001") {// 筒仓
			int lv = pDBItem->upgradeid() + 1;
			CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
			int slot = Tbl->slotCapacity;
			if(!updateMineralsLimit(slot))
				return false;
		}
	}
	else if(oldState == 1 && newState == 2 )//开始升级
	{
		pDBItem->set_time(ntime);
		pDBItem->set_updateat(time(NULL));
		pDBItem->set_incometorestore(incomeToRestore);
	}
	else if(oldState == 2 && newState == 1) //升级完成
	{
		int level = pDBItem->upgradeid() + 2;
		pDBItem->set_time(ntime);
		pDBPlanet->set_droidinuse(pDBPlanet->droidinuse() - 1);
		int energy = ResourceSilosTblInst::instance().GetEnergy(sku, level);
		pDBItem->set_energy(energy);
		if(sku == SKU_wonders_headquarters)
		{
			pDBPlanet->set_hqlevel(pDBPlanet->hqlevel() + 1);
		}
		pDBPlanet->set_droidinuse(pDBPlanet->droidinuse() - 1);

		if (pDBItem->sku() == "rs_002_001") {// 银行
			int lv = pDBItem->upgradeid() + 1;
			CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
			int slot = Tbl->slotCapacity;
			//修改 上限
			if(!updateCoinsLimit(slot))
				return false;
		}
		if (pDBItem->sku() == "rs_001_001") {// 筒仓
			int lv = pDBItem->upgradeid() + 1;
			CFG_ResourceSilos* Tbl = ResourceSilosTblInst::instance().GetInfo(lv);
			int slot = Tbl->slotCapacity;
			if(!updateMineralsLimit(slot))
				return false;
		}
	}
	else //oldstate = 1 newstate = 1收集资源相关
	{
		pDBItem->set_time(ntime);
		pDBItem->set_collecttime(time(NULL));
		pDBItem->set_incometorestore(0);
	}
	pDBItem->set_state(newState);
	pDBItem->set_updateat(time(NULL));

	return true;
}

void Player::FillinNpcs(RseObtainNpcList *lst)
{
	if(!CanUse() || !lst){
		return;
	}

	for(int i = 0; i < m_pdbPlayer->npcs_size(); i++){
		DB_NPC *pDBNpc = m_pdbPlayer->mutable_npcs(i);
		if(pDBNpc){
			MsgNpc* pMsgNpc = lst->add_npclist();
			if(pMsgNpc){
				pMsgNpc->set_hqlevel(pDBNpc->hqlevel());
				pMsgNpc->set_sku(SkuIDTblInst::instance().GetName(pDBNpc->sku()));
				pMsgNpc->set_xp(pDBNpc->exp());
			}
		}
	}
}

void Player::FillInMissoin(RseObtainUniverse *rse)
{
	if(!CanUse() || !rse){
		return;
	}
	MsgMissionState *pMission = rse->mutable_missions();
	if(pMission){
		pMission->set_readytostart(m_pdbPlayer->missions().readytostart());
		pMission->set_available(m_pdbPlayer->missions().available());
		pMission->set_completed(m_pdbPlayer->missions().completed());
		pMission->set_rewarded(m_pdbPlayer->missions().rewarded());
		for(int i = 0; i < m_pdbPlayer->missions().params_size(); i++){
			MsgMissionParam *pMsgParam = pMission->add_params();
			DB_MissionParam *pDBParam = m_pdbPlayer->mutable_missions()->mutable_params(i);
			if(pMsgParam && pDBParam){
				pMsgParam->set_sku(pDBParam->sku());
				pMsgParam->set_endtime(pDBParam->endtime());
				string progress = pDBParam->sku() + ":" + pDBParam->target();
				pMsgParam->set_progress(progress);
			}
		}
	}
}

void Player::FillInPlanet(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}
	
	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}
	MsgPlanet *PMsgPlanet = rse->add_planets();
	PMsgPlanet->set_startype(pPlanet->star().startype());
	PMsgPlanet->set_starid(pPlanet->star().starid());
	PMsgPlanet->set_starname(pPlanet->star().starname());
	PMsgPlanet->set_sku(pPlanet->star().sku());
	PMsgPlanet->set_planetid(pPlanet->id());
	PMsgPlanet->set_planettype(pPlanet->type());
	PMsgPlanet->set_coinslimit(pPlanet->coinslimit());
	PMsgPlanet->set_mineralslimit(pPlanet->minerallimit());
	//PMsgPlanet->set_coinslimit(99999999999);
	//PMsgPlanet->set_mineralslimit(99999999999);
	PMsgPlanet->set_hqlevel(pPlanet->hqlevel());
	PMsgPlanet->set_capital(pPlanet->capital());
#if 0
	optional int32 planetId = 1;//": 1,
	optional int32 planetType = 2;//": 0,
	optional string starName = 3;//": "塌圖嗣嫌",
	optional int32 coinsLimit = 4;//": 9000,
	optional int32 mineralsLimit = 5;//": 9000,
	optional int32 starId = 6;//": 2038,
	optional string sku = 7;//": "4:1:4",
	optional int32 starType = 8;//": 3,
	optional int32 HQLevel = 9;//": 0,
	optional int32 capital = 10;//": 1,
	repeated int32 Planet = 11;//": []
#endif

}

void Player::FillinItem(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}

	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}

	for(int i = 0; i < pPlanet->items_size(); i++){
		DB_Item *pDBItem = pPlanet->mutable_items(i);
		MsgBuildingItem *pMsgItem = rse->add_items();
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
			//pMsgItem->set_updateat(pDBItem->updateat());
			if(pDBItem->state() == 2)//升级中
			{
				time_t ltime = time(NULL);
				pMsgItem->set_time(pDBItem->time() - (ltime - pDBItem->updateat())*1000);

			}
			else if(pDBItem->state() == 0)
			{
				int ntime = pDBItem->time() - (time(NULL) - pDBItem->updateat()) * 1000;
				if(ntime < 0)
					ntime = 0;
				pMsgItem->set_time(ntime);
			}
			else//资源相关
			{	
				int ntime = pDBItem->time() - (time(NULL) - pDBItem->collecttime()) * 1000;
				if(ntime < 0)
					ntime = 0;
				pMsgItem->set_time(ntime);
			}
				
			pMsgItem->set_updatedat(pDBItem->updateat());
		}
	}
}

void Player::FillinShipyard(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}

	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}

	for (int i = 0; i < pPlanet->shipyard_size(); i++){
		DB_Shipyard *pDBShipyard = pPlanet->mutable_shipyard(i);
		if(pDBShipyard->slot_size()>0)
		{
			MsgShipyard *pMsgShipyard = rse->add_shipyards();
			if(pDBShipyard && pMsgShipyard){
				pMsgShipyard->set_sid(pDBShipyard->sid());
				pMsgShipyard->set_updatedat(pDBShipyard->updatedat());
				pMsgShipyard->set_unlockedslots(pDBShipyard->unlockedslots());
				pMsgShipyard->set_block(0);
				for(int i = 0; i < pDBShipyard->slot_size(); i++){
					if(i == 0)
					{
						DB_Slot *pDBSlot = pDBShipyard->mutable_slot(0);
						int leftTime = pDBSlot->timeleft() - (time(NULL) - pDBShipyard->updatedat())*1000;//下线后第一个兵造完了
						if(pDBSlot->skunum() == 1 && leftTime < 0)
						{
							pDBSlot->set_timeleft(0);
						}
					}
					DB_Slot *pDBSlot = pDBShipyard->mutable_slot(i);
					if(pDBSlot ){
						if(pDBSlot->skunum() <= 0)
						{
							continue;
						}
						else
						{
							MsgShipyardSlot *pMsgSlot = pMsgShipyard->add_shipyard();
							if(!pMsgSlot)
								continue;
							pMsgSlot->set_sku(pDBSlot->sku());
							pMsgSlot->set_num(pDBSlot->skunum());
							pMsgSlot->set_timeleft(pDBSlot->timeleft());
						}
							
						
						//if(0 == i && 1 == pDBSlot->skunum() && 0 == pDBSlot->timeleft()){
							//pMsgShipyard->set_block(1);
						//}
					}
				}
			}
		}
	}
}

void Player::FillinHangar(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}

	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}

	for (int i = 0; i < pPlanet->hangars_size(); i++){
		DB_Hangar *pDBHangar = pPlanet->mutable_hangars(i);
		MsgHangars *pMsgHangars = rse->add_hangars();
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
}

void Player::FillinBunker(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}

	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}

	for (int i = 0; i < pPlanet->bunkers_size(); i++){
		DB_Bunker *pDBBunker = pPlanet->mutable_bunkers(i);
		MsgBunker *pMsgBunker = rse->add_bunkers();
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
}

void Player::FillinGameUnit(RseObtainUniverse *rse, int nPlanetId)
{
	if(!CanUse() || !rse){
		return;
	}

	DB_Planet *pPlanet = GetPlanet(nPlanetId);
	if(!pPlanet){
		return;
	}

	for (int i = 0; i < pPlanet->units_size(); i++){
		DB_GameUnit *pDBUnit = pPlanet->mutable_units(i);
		MsgGameUnit *pMsgUnit = rse->add_gameunits();
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

void Player::FillinUniverse(RseObtainUniverse *rse, int nPlanetID)
{
	if(!CanUse() || !rse){
		return;		
	}
	rse->set_levelbasedonscore(1);
	rse->set_vip(m_pUser->GetIsYellowDmd());
	rse->set_yearvip(m_pUser->GetIsYearYellowDmd());
	rse->set_viplevel(m_pUser->GetYearYellowDmdLevel());
	rse->set_newplayergiftbag(false);
	rse->set_everydaygiftbag(false);
	rse->set_spycapsulestimeleft(0);
	rse->set_dcplayername(m_pUser->GetPlatformId());
	rse->set_dcworldname("");
	rse->set_dcplayerrank(1);
	//rse->set_levelbasedonscore(1);
	rse->set_dccoins(GetRes(RC_Coin));
	rse->set_dcminerals(GetRes(RC_Mineral));
	//rse->set_dccoins(999999999);
	//rse->set_dcminerals(999999999);
	rse->set_score(GetRes(RC_Score));
	rse->set_exp(GetRes(RC_Exp));
	rse->set_lastvisittime(m_pUser->GetLastLoginTime());
	rse->set_lastlevelnotified(0);
	rse->set_dccash(GetRes(RC_Cash));
	//rse->set_dccash(999999999);
	DB_Planet *pp = m_pdbPlayer->mutable_planets(0);//默认登录第一个星球
	rse->set_dcdroids(pp->droids());
	rse->set_dcdroidsinuse(0);
	rse->set_damageprotectiontimeleft(m_pdbPlayer->state().dmgprotectiontimeleft());
	rse->set_damageprotectiontimetotal(m_pdbPlayer->state().dmgprotectiontimetotal());
	rse->set_tutorialcompleted(m_pdbPlayer->state().tutorialcompleted());
	char flag[128];
	sprintf(flag, "quality:%d,music:%d,sound:%d,alliancesWelcomeId:%d,", m_pdbPlayer->flag().quality(), m_pdbPlayer->flag().music(), m_pdbPlayer->flag().effect(), m_pdbPlayer->flag().alliancewelcome());
	//sprintf(flag, "quality:%d,music:%d,sound:%d,alliancesWelcomeId:%d,", 1, 0, 0, 0);
	rse->set_flags(flag);
	FillInMissoin(rse);
	FillInPlanet(rse, nPlanetID);
	FillinItem(rse, nPlanetID);
	FillinShipyard(rse, nPlanetID);
	FillinHangar(rse, nPlanetID);
	FillinBunker(rse, nPlanetID);
	FillinGameUnit(rse, nPlanetID);
}

void Player::FillInWishList(RseObtainSocialItems* rse)
{
	if(!CanUse() || !rse){
		return;		
	}
	string wishlist = "";
	DB_WishItemList* pDBWIL = m_pdbPlayer->mutable_wishitemlist();
	for(int i = 0; i< pDBWIL->wishitem_size();i++)
	{
		DB_WishItem* pDBWI = pDBWIL->mutable_wishitem(i);
		if(pDBWI && (pDBWI->sku() != ""))
		{
			string tmp = "," + pDBWI->sku();
			wishlist.append(tmp);
		}
	}
	rse->set_wishlist(wishlist);
}
void Player::FillSocialItems(RseObtainSocialItems *rse)
{
	if(!CanUse() || !rse){
		return;		
	}
	for(int i = 0;i<m_pdbPlayer->socialitems_size();i++)
	{
		DB_SocialItem* pDBSI = m_pdbPlayer->mutable_socialitems(i);
		MsgItem* MI = rse->add_itemslist();
		int sku = 0;
		safe_atoi(pDBSI->sku(),sku);
		MI->set_sku(sku);
		//MI->set_position(pDBSI->);
		//MI->set_sequence(pDBSI->sq);
		MI->set_counter(pDBSI->counter());
		MI->set_quantity(pDBSI->amount());
		MI->set_timeleft(pDBSI->timeleft());
	}
	/*if(!bAdd)
	{
		for(int i = 1;i<80;i++)
		{
			CFG_Item* Tbl = ItemTblInst::instance().GetItem(i);
			string Sku = Tbl->sku;
			DB_SocialItem*pDBSI = NULL;
			if(i >= m_pdbPlayer->socialitems_size())
				DB_SocialItem*pDBSI = m_pdbPlayer->add_socialitems();
			if(!pDBSI)
				return;
			MsgItem* MI = rse->add_itemslist();
			int sku = 0;
			safe_atoi(Sku,sku);
			pDBSI->set_sku(Sku);
			pDBSI->set_timeleft(0);
			pDBSI->set_counter(0);
			pDBSI->set_amount(2);
			MI->set_sku(sku);
			MI->set_counter(pDBSI->counter());
			MI->set_quantity(pDBSI->amount());
			MI->set_timeleft(pDBSI->timeleft());
		}
		bAdd = true;
	}*/
	
}

void Player::FillinBookmarks(RseQueryStarsBookmarks *rse)
{
	if(!CanUse() || !rse){
		return;
	}

	for (int i = 0; i < m_pdbPlayer->bookmarks_size(); i++){
		DB_StarsBookmark *pDBBookmark = m_pdbPlayer->mutable_bookmarks(i);
		Bookmarks *pMsgBookmark = rse->add_bookmarks();
		if(pDBBookmark && pMsgBookmark){
			pMsgBookmark->set_sku(pDBBookmark->sku());
			pMsgBookmark->set_starid(pDBBookmark->starid());
			pMsgBookmark->set_starname(pDBBookmark->starname());
			pMsgBookmark->set_startype(pDBBookmark->startype());
		}
	}
}


void Player::NewBattle(int64 nTargetPlayer, int nPlanetId, time_t time)
{
	if(!m_pBattleManager || !CanUse()){
		return;
	}
	m_pBattleManager->NewBattle(nTargetPlayer, nPlanetId, time);
}

void Player::FillBattleData(RseObtainUniverse *pRsp)
{
	if(!CanUse() || !pRsp || !m_pBattleManager){
		return;
	}

	
}

void Player::FillinPvELite(RseQueryPvE* rsp)
{
	if(!CanUse() || !rsp){
		return;
	}

	for(int i = 0; i < m_pdbPlayer->pve_size(); i++){
		DB_KeyValue *pDBPvE = m_pdbPlayer->mutable_pve(i);
		MsgKeyValue *pMsgPvE = rsp->add_pve();
		if(pDBPvE && pMsgPvE){
			pMsgPvE->set_key(pDBPvE->key());
			pMsgPvE->set_value(pDBPvE->value());
		}
	}
}

