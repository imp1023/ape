#ifndef _WORLDEVENTHANDLER_H_
#define _WORLDEVENTHANDLER_H_
#pragma once
#include <log4cxx/logger.h>
#include "../event/EventHandler.h"

class Event;
class EventQueue;
class WorldDataHandler;
class WorldNetHandler;

class WorldEventHandler
{
public:
	WorldEventHandler(EventQueue *eq, WorldDataHandler *dh, WorldNetHandler *nh, int nid);
	~WorldEventHandler();

public:
	void start();
	EventHandler* getEventHandler(){ return eh_; }
	WorldDataHandler* getDataHandler() { return dh_; }
    int nid() {return nid_;}

	bool sendEventToGamed(Event *e, int gid);
    bool sendEventToGamedWithFd(Event *e, int fd);
	bool sendEventToUser(Event *e, int64 uid);
	bool sendEventToAllGamed(Event *e);
	void sendFdString(int fd, const string &content);

	inline EventQueue*  getEventQueue() {return eq_;}
private:
	void initialEventProcessors();

	EventQueue* eq_;
	WorldDataHandler* dh_;
	WorldNetHandler* nh_;
	EventHandler* eh_;
	int nid_;
	log4cxx::LoggerPtr logger_;
};

#endif
