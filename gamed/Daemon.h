#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <boost/smart_ptr.hpp>
using boost::scoped_ptr;
#include <log4cxx/logger.h>

class EventQueue;
class GameDataHandler;
class GameEventHandler;
class GameNetHandler;

class Daemon
{
public:
	Daemon();
	virtual ~Daemon();
	void		Init(int nid);
	void		start();
	void		quit();
	void		LoadConfig();
	void		ClearConfig();

	static Daemon&	Instance(){return m_Daemon;}

	GameDataHandler*	GetGameDataHandler() {return dh_.get();}
	GameEventHandler*	GetGameEventHandler() { return eh_.get();}

    int gameid() const { return nid_; }

private:
	static Daemon		m_Daemon;
    
    scoped_ptr<GameEventHandler> eh_;
    scoped_ptr<EventQueue>       eq_;
    scoped_ptr<GameDataHandler>  dh_;
    scoped_ptr<GameNetHandler>   nh_;
    int                 nid_;
	log4cxx::LoggerPtr	logger_;
};

#endif
