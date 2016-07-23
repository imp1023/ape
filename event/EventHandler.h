#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_
#pragma once

#include <pthread.h>
#include <vector>
#include "../common/const_def.h"

#ifndef _WIN32
#include <signal.h>
#endif
#include <log4cxx/logger.h>

#include "../common/ServerConfig.h"
#include "../common/statistics.h"
#include "../common/staticfile.h"
using namespace std;

class NetHandler;
class DataHandler;
class Event;
class EventQueue;
class Clock;
class Counter;

typedef void (*ProcessRoutine)(Event*);
extern ServerConfig serverConfig;

enum ServerEventType
{
	Srv_UnDefine = 0,
	Srv_Game,
	Srv_Country,
	Srv_Region,
	Srv_Fight,
	Srv_Rank,

};

class EventHandler
{
public:
	EventHandler(EventQueue* eq, DataHandler* dh, NetHandler* nh, int nid);
	~EventHandler(void);
	bool start(ServerEventType emType=Srv_UnDefine,time_t ltUpdateTime = 0);
	void quit();
	void postBackEvent(Event *e);
	void registHandler(int cmd, ProcessRoutine handler);
	static void urgentSaveHandler(int signo);
public:
	void setUrgentSaving(bool flag) {urgent_saving_flag_ = flag;}
	inline time_t getReversion() { return reversion_; }
	inline NetHandler* getNetHandler(){return nh_;}
	inline DataHandler* getDataHandler() {return dh_;}
	inline GameWorkingStatus getWorkingStatus() {return status_;}
	inline void setWorkingStatus(GameWorkingStatus status) {status_ = status;}
	inline void reportProfile() {event_pf_->report();}
	inline void clearProfile() {event_pf_->clear();}
	Profile* GetProfile(){return event_pf_;}
protected:
	inline void acquireEventLock() {pthread_mutex_lock(eq_mutex_);}
	inline void releaseEventLock() {pthread_mutex_unlock(eq_mutex_);}
	inline void acquireDataLock() {pthread_mutex_lock(data_mutex_);}
	inline void releaseDataLock() {pthread_mutex_unlock(data_mutex_);}
private:
	void dispatchEvent(Event *e);
	void run();
	static void* thread_fun(void* args);
	void tick();
	void safeQuit();
private:
	pthread_t		tid_;
	pthread_mutex_t *eq_mutex_;
	pthread_mutex_t *data_mutex_;
	EventQueue		*eq_;
	log4cxx::LoggerPtr logger_;
	Statistics		*stat_;
	Counter			* counter_;
	bool			urgent_saving_flag_;
	enum GameWorkingStatus status_;
	int				nid_;
	Profile			*event_pf_;
	time_t			last_check_wait_event_;
	vector<Event*>	wait_event_list_;
	time_t			m_lLastCheckTime;
	time_t			m_ltUpdateTime;
	static const int maxProcessRoutines = 3096;
	ProcessRoutine	processRoutines_[maxProcessRoutines];
	time_t			reversion_;
	NetHandler		*nh_;
	DataHandler		*dh_;
	ServerEventType	m_emSrvType;
	bool			m_IsSendOnceMsg;
	int				m_nTimerTick;

	int				m_nTimerTickLuckyNum;

public:
	StaticFile m_xEventCounter;
	time_t        m_lstEventTime;
};

#endif
