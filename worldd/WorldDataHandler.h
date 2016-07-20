#ifndef _WORLDDATAHANDLER_H_
#define _WORLDDATAHANDLER_H_
#pragma once

#include <log4cxx/logger.h>
#include <vector>
#include "../net/NetHandler.h"
#include "../event/DataHandler.h"
using namespace std;

class DataHandler;

class WorldDataHandler : public DataHandler
{
public:
	WorldDataHandler(int nid);
	virtual ~WorldDataHandler() {}

	GameWorkingStatus setWorkingStatus(int gid, GameWorkingStatus status);
	int getGameIdfromPlatId(string const& platid,int nRegion);
	int getGamedIdByUserId(int64 uid);
	virtual void tick() {}
private:
	void checkAllWorkingStatus();
private:
	vector<enum GameWorkingStatus> game_status_;
	bool game_all_ready_;
};

#endif
