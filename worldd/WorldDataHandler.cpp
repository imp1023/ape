#include "../event/DataHandler.h"
#include "WorldDataHandler.h"
#include "../common/ServerConfig.h"
#include "../common/distribution.h"

extern ServerConfig serverConfig;

WorldDataHandler::WorldDataHandler(int nid)
:DataHandler(nid)
{
	game_status_.resize(serverConfig.gamedNum()+1, NOT_CONNECTED);
	game_status_[0] = NORMAL;
	game_all_ready_ = false;
}

int WorldDataHandler::getGameIdfromPlatId(string const& platid,int nRegion)
{
	int hash = getPlatidHash(platid);
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = serverConfig.GetPhysicsRegionByLogicRegion(nRegion);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;
}

int WorldDataHandler::getGamedIdByUserId(int64 uid)
{
	int hash = getUidHash(uid);
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = getPhysicsRegion(uid);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;

	//return hash % serverConfig.gamedNum() + 1;
}

GameWorkingStatus WorldDataHandler::setWorkingStatus(int gid, GameWorkingStatus status)
{
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

