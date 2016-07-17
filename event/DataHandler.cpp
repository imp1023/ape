#define _CRT_SECURE_NO_WARNINGS
#include "DataHandler.h"
#include "../common/distribution.h"
#include "../common/ServerConfig.h"

using namespace std;

DataHandler::DataHandler(int nid)
{
	logger_ = log4cxx::Logger::getLogger("DataHandler");
	nid_ = nid;
	revision_ = 0;
	pthread_mutex_init(&mutex_, NULL);
}

DataHandler::~DataHandler(void)
{
	pthread_mutex_destroy(&mutex_);
}


void DataHandler::init()
{
}


void DataHandler::tick()
{
}

void DataHandler::quit()
{
}

void DataHandler::termThreads()
{
}



