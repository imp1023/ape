#include "GameConfigMgr.h"
#include "GameServerConfig.h"
//table
#include "CountryNameTbl.h"
#include "InitItemTbl.h"
#include "GameUnitsTbl.h"
#include "SkuIdTbl.h"
#include "NpcInitItemTbl.h"
#include "ItemSumTbl.h"
#include "FlashVersionCfg.h"
#include "GameConstantSetCfg.h"

GameConfigMgr* GameConfigMgr::_sMgr = NULL;

GameConfigMgr::GameConfigMgr(void)
{
}

GameConfigMgr::~GameConfigMgr(void)
{
}

void GameConfigMgr::LoadGameConfigInfo()
{
	FlashVersionCfg::Instance().LoadTbl();
	CountryNameTblInst::instance().LoadInfo();
	InitItemTblInst::instance().LoadInfo();
	GameUnitTblInst::instance().LoadInfo();
	SkuIDTblInst::instance().LoadInfo();
	NPCInitItemTblInst::instance().LoadInfo();
	ItemSumTblInst::instance().LoadInfo();
	GameConstantSetCfg::Instance().LoadTbl();
	fflush(stdout);
}

void GameConfigMgr::ClearGameConfigInfo()
{
	FlashVersionCfg::Instance().ClearTbl();
	CountryNameTblInst::instance().Clear();
	InitItemTblInst::instance().Clear();
	GameUnitTblInst::instance().Clear();
	SkuIDTblInst::instance().Clear();
	NPCInitItemTblInst::instance().Clear();
	ItemSumTblInst::instance().Clear();
	GameConstantSetCfg::Instance().ClearTbl();
}
