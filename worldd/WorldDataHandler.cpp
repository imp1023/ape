#include "../event/DataHandler.h"
#include "WorldDataHandler.h"
#include "../common/ServerConfig.h"
#include "../common/distribution.h"
#include "../common/DateTime.h"
#include "worldd.h"


WorldDataHandler::WorldDataHandler(int nid)
:DataHandler(nid)
{
	game_status_.resize(ServerConfig::Instance().gamedNum()+1, NOT_CONNECTED);
	game_status_[0] = NORMAL;
	game_all_ready_ = false;

// 	if (GlobalCampBattleLevelTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		CenterCampBattleModuleInst::instance().Load();
//     GlobalDbData::instance().Load();
// 	if (globalArenaServerTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		GlobalArenaModuleInst::instance().Load();
// 	int allworldid = globalArenaBaseDataTblInst::instance().getAllGlobalArenaWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		AllGlobalArenaModuleInst::instance().Load();
// 	//StageTowerWorldModuleInst::instance().load();
// 	CommonSystemMgrInst::instance().Load();
// 	allworldid = globalGuildBattleBaseDataTblInst::instance().getGlobalGuildBattleWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		GlobalGuildBattleModuleInst::instance().Load();
}

GameWorkingStatus WorldDataHandler::setWorkingStatus(int gid, GameWorkingStatus status)
{
    if (gid <= 0 || gid > ServerConfig::Instance().gamedNum())
    {
        return NORMAL;
    }

	GameWorkingStatus old_status = game_status_[gid];
	if(status == NORMAL)
	{
		game_status_[gid] = NORMAL;
		checkAllWorkingStatus();
	}
	else
	{
		game_status_[gid] = status;
		game_all_ready_ = false;
	}
	return old_status;
}

void WorldDataHandler::checkAllWorkingStatus()
{
	bool ready = true;
	for (size_t i = 0; i < game_status_.size(); i ++)
	{
		if (game_status_[i] != NORMAL)
		{
			ready = false;
			break;
		}
	}
	if (!game_all_ready_ && ready)
	{
		game_all_ready_ = true;
		LOG4CXX_INFO(logger_, "All game server already.");
	}
}

void WorldDataHandler::tick()
{
// 	if (GlobalCampBattleLevelTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		CenterCampBattleModuleInst::instance().Tick();
//     GlobalDbData::instance().Tick();
//     ChallengeWorldModuleInst::instance().Tick();
// 	if (globalArenaServerTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		GlobalArenaModuleInst::instance().Tick();
// 	int allworldid = globalArenaBaseDataTblInst::instance().getAllGlobalArenaWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		AllGlobalArenaModuleInst::instance().Tick();
// 	//StageTowerWorldModuleInst::instance().tick();
// 	CommonSystemMgrInst::instance().Tick();
// 	allworldid = globalGuildBattleBaseDataTblInst::instance().getGlobalGuildBattleWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		GlobalGuildBattleModuleInst::instance().Tick();
}

void WorldDataHandler::quit()
{
//     GlobalDbData::instance().SaveAll();
// 	if (globalArenaServerTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		GlobalArenaModuleInst::instance().SaveAll();
// 	int allworldid = globalArenaBaseDataTblInst::instance().getAllGlobalArenaWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		AllGlobalArenaModuleInst::instance().SaveAll();
// 	//StageTowerWorldModuleInst::instance().saveAll();
// 	CommonSystemMgrInst::instance().SaveAll();
// 	allworldid = globalGuildBattleBaseDataTblInst::instance().getGlobalGuildBattleWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		GlobalGuildBattleModuleInst::instance().SaveAll();

}



