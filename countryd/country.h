#pragma once

#include <log4cxx/logger.h>

class EventQueue;
class CountryNetHandler;
class CountryEventHandler;
class CountryDataHandler;

class CountrySrv {
public:
	CountrySrv();
	~CountrySrv();
	void start();

private:
	EventQueue *eq_;
	CountryNetHandler *nh_;
	CountryEventHandler *eh_;
	CountryDataHandler *dh_;
	log4cxx::LoggerPtr logger_;
};
