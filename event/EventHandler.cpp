#define _CRT_SECURE_NO_WARNINGS
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif
#include <time.h>
#include <pthread.h>
#include "EventHandler.h"
#include "DataHandler.h"
#include "../common/ServerConfig.h"
#include "../net/NetHandler.h"
#include "../common/counter.h"
#include "../common/string-util.h"
#include "../common/statistics.h"
#include "../common/TicTac.h"
#include "../common/Clock.h"
#include "../event/EventQueue.h"
#include "../common/SysLog.h"
#include "../common/Msg2QQ.h"
#include "../event/EventDefine.h"
//#include "../logic/SysBroadcast_T.h"
EventHandler* global_eh;
extern int g_processingCmd;
//extern SysBroadcast_Table g_SysBroadcastT;

EventHandler::EventHandler(EventQueue* eq, DataHandler* dh, NetHandler* nh, int nid) 
{
	global_eh = this;
	nid_ = nid;
	logger_ = log4cxx::Logger::getLogger("EventHandler");

	m_emSrvType = Srv_UnDefine;
	eq_ = eq;
	dh_ = dh;
	nh_ = nh;
	eq_mutex_ = eq->mutex();
	data_mutex_ = dh->mutex();
	m_lLastCheckTime = 0;
	m_ltUpdateTime = 0;
	status_ = NORMAL;
	stat_ = new Statistics();
	event_pf_ = new Profile();
	counter_ = new Counter();
	urgent_saving_flag_ = false;
	reversion_ = 0;
	m_lstEventTime = 0;
	for (int i=0; i<maxProcessRoutines; i++) processRoutines_[i] = NULL;
	m_IsSendOnceMsg = true;
	m_nTimerTick = 0;
	m_nTimerTickLuckyNum =0;
	m_emSrvType = Srv_UnDefine;
}

EventHandler::~EventHandler(void)
{
	delete stat_;
	delete counter_;
	delete event_pf_;
}

bool EventHandler::start(ServerEventType emType/* =Srv_UnDefine */,time_t ltUpdateTime /* = 0 */)
{
	m_emSrvType = emType;
	m_ltUpdateTime = ltUpdateTime;
#ifdef _WIN32
	DWORD threadid;
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EventHandler::thread_fun,
		(void*)this, 0, &threadid) == NULL;
#else
	return pthread_create(&tid_, NULL, &EventHandler::thread_fun, (void*)this)==0;
#endif
}

void EventHandler::quit()
{
	pthread_join(tid_, NULL);
}

void* EventHandler::thread_fun(void *args) 
{
#ifndef _WIN32
	sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGINT);
	int err;
	err = pthread_sigmask(SIG_UNBLOCK, &sig_mask, NULL);
	struct sigaction save_action;
	save_action.sa_handler = &EventHandler::urgentSaveHandler;
	save_action.sa_flags = SA_RESTART;
	// turn on/off by the following two lines
	if (err==0) err = sigaction(SIGINT, &save_action, NULL);
	// end
	if (err==0) {
#endif
		global_eh = static_cast<EventHandler*>(args);
		global_eh->run();
#ifndef _WIN32
		pthread_exit(NULL);
	} else {
		printf("register signal failed in thread.\n");
	}
#endif
	return NULL;
}

void EventHandler::run() 
{
	//status_ = NORMAL;
	//eq_->pushSaveData(nid_, 1, -1, clock_->refreshNow()+30*1000, 0);
	//long interval = gameConfig.gamed_heartBeatInterval(nid_);
	long interval = 20; // 20ms
#ifdef _WIN32
	int dummyfd = socket(PF_INET, SOCK_STREAM, 0);
	fd_set dummyread;
	FD_ZERO(&dummyread);
	TIMEVAL timeout;
#else
	timeval timeout;
#endif
	timeout.tv_sec = 0;
	timeout.tv_usec = interval * 1000;
	vector<Event*> events;
	events.reserve(40960);
	last_check_wait_event_ = Clock::getCurrentSystemTime();
	TicTac timer;
	TicTac event_watch;
	while (true) {
		if(nh_ == NULL) {
			usleep(100);
			continue;
		}
		timer.tic();
		reversion_ = Clock::getCurrentSystemTime();
		if(dh_ != NULL) {
			dh_->setRevision(reversion_);
		}
		CHG_LOG_FILE();

#ifndef _WIN32
		int ntime =  reversion_;
		if (reversion_ - m_lstEventTime>60000)
		{
			m_xEventCounter.WriteFirstLine(ntime);
			m_lstEventTime = reversion_;
		}
			
#endif

		acquireEventLock();
		for (size_t i=0; i<events.size(); i++) {
			events[i]->Clear();
			eq_->freeEvent(events[i]);
		}
		events.clear();
		if (status_!=NORMAL) {
			while (!eq_->isEmpty() && 0 > eq_->topTime()) {
				events.push_back(eq_->popEvent());
			}
		} else {
			while (!eq_->isEmpty() && reversion_ >= eq_->topTime()) {
				events.push_back(eq_->popEvent());
			}
		}
		releaseEventLock();

		if (!urgent_saving_flag_) 
		{
			tick();
			if(last_check_wait_event_ < reversion_ - 300 && !wait_event_list_.empty())
			{
				events.insert(events.end(), wait_event_list_.begin(), wait_event_list_.end());
				wait_event_list_.clear();
				last_check_wait_event_ = reversion_;
			}
			event_watch.tic();
			for (size_t i=0; i<events.size(); i++) 
			{
				acquireDataLock();
				dispatchEvent(events[i]);
				releaseDataLock();
				event_pf_->hitEvent(events[i]->cmd(), (double)event_watch.tac());
			}
		} 
		else
		{
			printf("safeQuit before\n");
			acquireDataLock();
			safeQuit();
			releaseDataLock();
			printf("safeQuit after\n");
			dh_->termThreads();
			printf("termThreads\n");
			CSysLog::GetInstance()->Quit();
			printf("CSysLog\n");
			nh_->quit();
			printf("net quit\n");
			break;
		}

		stat_->capture("event.num", (float)events.size());
		int elapsed = (int)timer.tac();
		stat_->capture("event.process-time", (float)elapsed);
		stat_->capture("user.online", (float)counter_->count("user.online"));

		if (elapsed>=interval) 
		{
			if (elapsed>=10*interval)
				LOG4CXX_WARN(logger_, "Heart beat overload, used "<<elapsed<<" ms. (interval:"<<interval<<" ms)");
			continue;
		}
		timeout.tv_usec = (interval - elapsed) * 1000;
#ifdef _WIN32
		FD_SET(dummyfd, &dummyread);
		int r;
		if ((r=select(dummyfd+1, &dummyread, NULL, NULL, &timeout))<0) {
			int err = WSAGetLastError();
			LOG4CXX_ERROR(logger_, "select failed in thread, return value:"<<r<<" error no:"<<err);
		}
#else
		int r;
		if ((r=select(1, NULL, NULL, NULL, &timeout))<0) {
			LOG4CXX_ERROR(logger_, "select failed in thread, return value:"<<r);
		}
#endif
	}
	exit(0);
}

void EventHandler::dispatchEvent(Event *e) 
{
	if(!e->has_cmd()||e->cmd()<0||e->cmd()>=maxProcessRoutines)
	{
		return;
	}
	if (e->IsInitialized() && processRoutines_[e->cmd()]!=NULL)
	{
		g_processingCmd = e->cmd();
		(*processRoutines_[e->cmd()])(e);
		g_processingCmd = 0;
	}
}

void EventHandler::urgentSaveHandler(int signo) 
{
	printf("Catching signal SIGINT. Terminating...");
	global_eh->setUrgentSaving(true);
}

void EventHandler::postBackEvent(Event* e)
{
	Event* ne = eq_->allocateEvent();
	ne->CopyFrom(*e);
	ne->set_time(reversion_+1000);
	wait_event_list_.push_back(ne);
	LOG4CXX_DEBUG(logger_, "Push back event:"<<ne->cmd());
}

void EventHandler::tick()
{
	switch(m_emSrvType)
	{
	case Srv_UnDefine:
		break;
	case Srv_Game:
		{
			if(m_ltUpdateTime > 0)
			{
				if(reversion_ - m_lLastCheckTime > m_ltUpdateTime)
				{
					m_lLastCheckTime = reversion_;
					Event* e = eq_->allocateEvent();
					e->set_cmd(EVENT_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);

					/*m_nTimerTick ++;
					if(m_nTimerTick >= 5*60)
					{
						Event* e = eq_->allocateEvent();
						e->set_cmd(EVENT_TIMER_5_TICK);
						e->set_state(Status_Normal_Game);
						e->set_time(0);
						eq_->safePushEvent(e);
						m_nTimerTick = 0;
					}

					m_nTimerTickLuckyNum++;
					if(m_nTimerTickLuckyNum >= 60)
					{
						Event* e = eq_->allocateEvent();
						e->set_cmd(EVENT_LUCKY_NUM);
						e->set_state(Status_Normal_Game);
						e->mutable_luckynummsg()->set_type( 1 );//·¢ËÍÊý¾Ý
						e->set_time(0);
						eq_->safePushEvent(e);
						m_nTimerTickLuckyNum = 0;
					}*/
				}
			}
			/*if (m_IsSendOnceMsg)
			{
				m_IsSendOnceMsg = false;

				Event* e = eq_->allocateEvent();
				if (e)
				{
					e->set_cmd(EVENT_USER_HOURMETER_BRAODCAST_START);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);
				}

				e = eq_->allocateEvent();
				if (e)
				{
					e->set_cmd(EVENT_UPDATE_BROADCAST_ROBOT);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					e->mutable_updatebroadcastrobot()->set_nype(BROADCAST_ROBOT_MSG_UPDATE);
					eq_->safePushEvent(e);
				}

				e = eq_->allocateEvent();
				e->set_cmd(EVENT_TIMER_5_TICK);
				e->set_state(Status_Normal_Game);
				e->set_time(0);
				eq_->safePushEvent(e);
			}*/
		}
		break;
	case Srv_Country:
		{
			if(m_ltUpdateTime > 0)
			{
				if(reversion_ - m_lLastCheckTime > m_ltUpdateTime)
				{
					m_lLastCheckTime = reversion_;
					Event* e = eq_->allocateEvent();
					e->set_cmd(EVENT_COUNTRY_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);
				}
			}
		}
		break;
	/*case Srv_Region:
		{
			if(m_ltUpdateTime > 0)
			{
				if(reversion_ - m_lLastCheckTime > m_ltUpdateTime)
				{
					m_lLastCheckTime = reversion_;
					Event* e = eq_->allocateEvent();
					e->set_cmd(EVENT_REGION_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);

					e = eq_->allocateEvent();
					e->set_cmd(EVENT_REGIONCOUNTRY_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);
				}
			}
		}
		break;
	case Srv_Fight:
		{
			if(m_ltUpdateTime > 0)
			{
				if(reversion_ - m_lLastCheckTime > m_ltUpdateTime)
				{
					m_lLastCheckTime = reversion_;
					Event* e = eq_->allocateEvent();
					e->set_cmd(EVENT_FIGHT_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);

				}
			}
		}
		break;
	case Srv_Rank:
		{
			if(m_ltUpdateTime > 0)
			{
				if(reversion_ - m_lLastCheckTime > m_ltUpdateTime)
				{
					m_lLastCheckTime = reversion_;
					Event* e = eq_->allocateEvent();
					e->set_cmd(EVENT_RANK_TIMER);
					e->set_state(Status_Normal_Game);
					e->set_time(0);
					eq_->safePushEvent(e);

				}
			}
		}
		break;*/
	default:
		break;
	}
	
	if(dh_ != NULL)
	{
		dh_->tick();
	}
}

void EventHandler::safeQuit()
{
	if(dh_ != NULL)
	{
		dh_->quit();
	}
}

void EventHandler::registHandler(int cmd, ProcessRoutine handler)
{
	processRoutines_[cmd] = handler;
}
