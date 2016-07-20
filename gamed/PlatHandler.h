#pragma once

#include <log4cxx/logger.h>
#include "../net/ProtocolHandler.h"
#include <string>
using namespace std;

class EventQueue;
class NetHandler;

class PlatHandler:public ProtocolHandler{
public:
	PlatHandler(EventQueue *eq, int fd, NetHandler *nh);
	~PlatHandler();
	void handle(int64 uid, string &req);
	int handlerType() {return ProtocolHandler::WORLD;}

protected:
	int fd_;
	EventQueue *eq_;
	NetHandler *nh_;
	log4cxx::LoggerPtr logger_;
};

