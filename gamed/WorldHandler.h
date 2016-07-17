#ifndef _WORLDHANDLER_H_
#define _WORLDHANDLER_H_
#pragma once
#include <string>
#include <log4cxx/logger.h>
#include "../net/ProtocolHandler.h"

using namespace std;

class EventQueue;
class NetHandler;

class WorldHandler:public ProtocolHandler
{
public:
	WorldHandler(EventQueue *eq, int fd, NetHandler *nh);
	~WorldHandler();
	void handle(int64 uid, string &req);
	int handlerType() {return ProtocolHandler::WORLD;}

protected:
	int fd_;
	EventQueue *eq_;
	NetHandler *nh_;
	log4cxx::LoggerPtr logger_;
};

#endif
