#pragma once
#include <log4cxx/logger.h>

class EventQueue;
class PlatNetHandler;
class PlatEventHandler;
class PlatDataHandler;

class PlatSrv {
public:
	PlatSrv();
	~PlatSrv();
	void start();

private:
	EventQueue *eq_;
	PlatNetHandler *nh_;
	PlatEventHandler *eh_;
	PlatDataHandler *dh_;
	log4cxx::LoggerPtr logger_;
};
