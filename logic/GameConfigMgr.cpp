#include "GameConfigMgr.h"
#include "GameServerConfig.h"
//table
#include "InitItemTbl.h"
#include "GameUnitsTbl.h"
#include "SkuIdTbl.h"
#include "NpcInitItemTbl.h"
#include "ItemSumTbl.h"
#include "FlashVersionCfg.h"
#include "GameConstantSetCfg.h"
#include "ResourceSilos.h"
#include "HangarTbl.h"
#include "LaboratoryTbl.h"
#include "ResourcesTbl.h"
#include "DefensesTbl.h"
#include "ShipyardsTbl.h"
#include "MissionsTbl.h"
#include "WonderDefinitionsTbl.h"
#include "ItemTbl.h"
#include "CollectablesTbl.h"
#include "CountryNameTbl.h"
#include "NPCInfoCfg.h"

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
	InitItemTblInst::instance().LoadInfo();
	CountryNameTblInst::instance().LoadInfo();
	GameUnitTblInst::instance().LoadInfo();
	SkuIDTblInst::instance().LoadInfo();
	MissionsTblInst::instance().LoadInfo();
	NPCInitItemTblInst::instance().LoadInfo();
	ItemSumTblInst::instance().LoadInfo();
	GameConstantSetCfg::Instance().LoadTbl();
	ResourceSilosTblInst::instance().LoadInfo();
	HangarTblInst::instance().LoadInfo();
	LaboratoryTblInst::instance().LoadInfo();
	ResourceTblInst::instance().LoadInfo();
	DefenseTblInst::instance().LoadInfo();
	ShipyardTblInst::instance().LoadInfo();
	WonderDefinitionsTblInst::instance().LoadInfo();
	ItemTblInst::instance().LoadInfo();
	CollectablesTblInst::instance().LoadInfo();
	NPCInfoCfg::Instance().LoadTbl();
	fflush(stdout);
}

void GameConfigMgr::ClearGameConfigInfo()
{
	FlashVersionCfg::Instance().ClearTbl();
	InitItemTblInst::instance().Clear();
	CountryNameTblInst::instance().Clear();
	GameUnitTblInst::instance().Clear();
	SkuIDTblInst::instance().Clear();
	MissionsTblInst::instance().Clear();
	NPCInitItemTblInst::instance().Clear();
	ItemSumTblInst::instance().Clear();
	GameConstantSetCfg::Instance().ClearTbl();
	ResourceSilosTblInst::instance().Clear();
	HangarTblInst::instance().Clear();
	LaboratoryTblInst::instance().Clear();
	ResourceTblInst::instance().Clear();
	DefenseTblInst::instance().Clear();
	ShipyardTblInst::instance().Clear();
	WonderDefinitionsTblInst::instance().Clear();
	ItemTblInst::instance().Clear();
	CollectablesTblInst::instance().Clear();
	NPCInfoCfg::Instance().ClearTbl();
}
