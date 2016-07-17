#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#pragma once


#include <log4cxx/logger.h>
#include <pthread.h>
#include "../common/const_def.h"

class DataHandler
{
public:
	DataHandler(int nid);
	virtual ~DataHandler(void);

	inline pthread_mutex_t* mutex() {return &mutex_;}
	inline time_t Revision() { return revision_; }
	inline void setRevision(time_t revision) { revision_ = revision; }

    virtual void init();
	virtual void tick();
	virtual void quit();
	virtual	void termThreads();

protected:
	pthread_mutex_t mutex_;
	log4cxx::LoggerPtr logger_;
	time_t revision_;
	int nid_;
};

#endif
