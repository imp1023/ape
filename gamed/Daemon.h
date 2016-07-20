#ifndef _DAEMON_H_
#define _DAEMON_H_
#pragma once
#include <log4cxx/logger.h>

class EventQueue;
class GameDataHandler;
class GameEventHandler;
class GameNetHandler;

class Daemon
{
public:
	Daemon(int nid);
	virtual ~Daemon();
	void		start();
	void		quit();
	void		LoadConfig();
	void		ClearConfig();
private:
	GameEventHandler	*eh;
	EventQueue			*eq;
	GameDataHandler		*dh;
	GameNetHandler		*nh;
	log4cxx::LoggerPtr	logger_;
};

#endif
