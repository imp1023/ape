#ifndef _GAMEHANDLER_H_
#define _GAMEHANDLER_H_

#pragma once
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"

class WorldNetHandler;

class GameHandler:public ProtocolHandler {
public:
	GameHandler(WorldNetHandler* nh, int fd);

	void handle(int64 uid, string &cmd);
	int handlerType() {return ProtocolHandler::GAMED;}
	void leave(int64 nid);

private:
	WorldNetHandler *nh_;
	int fd_;
};

#endif
