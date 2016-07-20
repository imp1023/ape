#ifndef _WORLD_H_
#define _WORLD_H_
#pragma once
#include <log4cxx/logger.h>

class EventQueue;
class WorldNetHandler;
class WorldEventHandler;
class WorldDataHandler;

class World {
public:
	World();
	~World();
	void start();

private:
	EventQueue *eq_;
	WorldNetHandler *nh_;
	WorldEventHandler *eh_;
	WorldDataHandler *dh_;
	log4cxx::LoggerPtr logger_;
};

#endif
