#pragma once
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"

class CountryNetHandler;

class GameHandler:public ProtocolHandler {
public:
	GameHandler(CountryNetHandler* nh, int fd);

	void handle(int64 uid, string &cmd);
	int handlerType() {return ProtocolHandler::GAMED;}
	void leave(int64 nid);

private:
	CountryNetHandler *nh_;
	int fd_;
};

