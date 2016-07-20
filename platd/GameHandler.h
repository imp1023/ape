#pragma once
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"

class PlatNetHandler;

class GameHandler:public ProtocolHandler {
public:
	GameHandler(PlatNetHandler* nh, int fd);

	void handle(int64 uid, string &cmd);
	int handlerType() {return ProtocolHandler::GAMED;}
	void leave(int64 nid);

private:
	PlatNetHandler *nh_;
	int fd_;
};

