#pragma once

#include <log4cxx/logger.h>
#include "../event/EventHandler.h"

class Event;
class EventQueue;
class CountryDataHandler;
class CountryNetHandler;
class CountryRgnDatHandler;
class DB_C_City;
class CCity;
class DB_C_User;

class CountryEventHandler
{
public:
	CountryEventHandler(EventQueue *eq, CountryDataHandler *dh, CountryNetHandler *nh, int nid);
	~CountryEventHandler();

public:
	void start();
	EventHandler* getEventHandler(){ return eh_; }
	CountryDataHandler* getDataHandler() { return dh_; }
	EventQueue* getEventQueue() { return eq_; }
	int	 GetCountrySrvID() {return nid_;}

	// Helper
	bool sendEventToGamed(Event *e, int gid);
	bool sendEventToAllGamed(Event *e);
	void sendFdString(int fd, const string &content);

	int getGamedIdByUserId(int64 uid);

	inline time_t getReversion() {return eh_->getReversion();}
	int GetSrvID() {return nid_;}

public:
	void SendCountryLiteInfo(int nGameID = -1);

public:
	inline void postBackEvent(Event* e)
	{
		eh_->postBackEvent(e);
	}

public:
private:
	void initialEventProcessors();

	EventQueue* eq_;
	CountryDataHandler* dh_;
	CountryNetHandler* nh_;
	EventHandler* eh_;
	int nid_;
	log4cxx::LoggerPtr logger_;
};

