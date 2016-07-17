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
	void Init(int wid);
	void		LoadConfig();
	void		ClearConfig();
	static World&	Instance(){return m_World;}
	inline int worldid() const { return wid_; }
	int			centerID();

	WorldEventHandler*	GetWorldEventHandler() { return eh_;}
private:
	static World		m_World;

	EventQueue *eq_;
	WorldNetHandler *nh_;
	WorldEventHandler *eh_;
	WorldDataHandler *dh_;
	log4cxx::LoggerPtr logger_;

    int wid_;
};

#endif
