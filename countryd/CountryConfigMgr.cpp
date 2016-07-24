#include "CountryConfigMgr.h"

#include "../logic/GameSrvCfg.h"
#include "../logic/FriendInfoCfg.h"
#include "../logic/FriendGiftCfg.h"
#include "../logic/CountryInfoCfg.h"
#include "../logic/GuildInfoCfg.h"
#include "../logic/StoreItemCfg.h"
#include "MemCacheServerHandler.h"
#include "../logic/InviteFrdGiftCfg.h"
#include "../logic/ArenaNpcInitConfig.h"
#include "../logic/ArenaFightRule.h"
#include "../logic/ArenaActive.h"
#include "../logic/WorldArenaFightRule.h"
#include "../logic/WorldArenaActive.h"
#include "../logic/WorldArenaNpcInital.h"
#include "../logic/GuildBtlDBCfg.h"
#include "../logic/OfficerInfoCfg.h"
#include "../logic/GameConstantSetCfg.h"
#include "../logic/GVEInfoCfg.h"
#include "../logic/HeroArenaFightRule.h"
#include "../logic/HeroArenaActive.h"
#include "../logic/HeroArenaNpcInital.h"
#include "../logic/ResourceCfg.h"
#include "../logic/GvgCfg.h"
#include "../logic/WeaponRankActiveCfg.h"
#include "../logic/WarGameCfg.h"
#include "../logic/LeagueBattleCfg.h"
#include "../logic/BossBtlCfg.h"
#include "../logic/GuildRedEnvelopeCfg.h"
#include "../logic/NpcActiveCfg.h"
#include "../logic/NewRegionActivityCfg.h"
#include "../logic/GameActivityTimeCfg.h"
#include "../logic/OneYuanPurchaseCfg.h"


CountryConfigMgr* CountryConfigMgr::_sMgr = NULL;

CountryConfigMgr::CountryConfigMgr(int nCountrySrvID)
{

}

CountryConfigMgr::~CountryConfigMgr(void)
{
}

void CountryConfigMgr::LoadCountryConfigInfo(int nCountrySrvID,bool bRootLoad/* =true */)
{
	bool bSuc = true;
	if(!GameSrvCfg::Instance().LoadTbl()
		||!StoreItemCfg::Instance().LoadTbl()
		||!CountryInfoCfg::Instance().LoadTbl()
		||!ArenaNpcInital::Instance().LoadTbl()
		||!ArenaFightRule::Instance().LoadTbl()
		||!ArenaConfig_Active::Instance().LoadTbl()
		||!WorldArenaActive::Instance().LoadTbl()
		||!WorldArenaFightRule::Instance().LoadTbl()
		||!WorldArenaNpcInital::Instance().LoadTbl()
		||!GuildInfoCfg::Instance().LoadTbl()
		||!OfficerInfoCfg::Instance().LoadTbl()
		||!GuildBtlDBCfg::Instance().LoadTbl()
		||!GameConstantSetCfg::Instance().LoadTbl()
		||!GVEInfoCfg::Instance().LoadTbl()
		||!HeroArenaActive::Instance().LoadTbl()
		||!HeroArenaFightRule::Instance().LoadTbl()
		||!HeroArenaNpcInital::Instance().LoadTbl()
		||!ResourceCfg::Instance().LoadTbl()
		||!GvgCfg::Instance().LoadTbl()
		||!WeaponRankActiveCfg::Instance().LoadTbl()
        ||!WarGameCfg::Instance().LoadTbl()
		||!LeagueBattleCfg::Instance().LoadTbl()
		||!BossBtlCfg::Instance().LoadTbl()
        ||!GuildRedEnvelopeCfg::Instance().LoadTbl()
		||!NpcActiveCfg::Instance().LoadTbl()
		||!NewRegionActivityCfg::Instance().LoadTbl()
		||!GameActivityTimeCfg::Instance().LoadTbl()
		||!OneYuanPurchaseCfg::instance().LoadTbl()
		
		)
	{
		printf("Read Config File ERROR!!!!!\r\n");
		exit(0);
	}

	MemCacheServerHandler::GetInst()->LoadConfig();
	MemCacheServerHandler::GetInst()->SetEnbale(GameSrvCfg::Instance().CountryCacheEnable() != 0);
	
	printf("Country Read Config File Success!!!!!!!!!!!\r\n");
	
}
