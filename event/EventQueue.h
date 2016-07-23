#ifndef _EVENTQUEUE_H_
#define _EVENTQUEUE_H_

#pragma once
#include <log4cxx/logger.h>
#include <string>
#include <queue>
#include "../common/const_def.h"
#include "event.pb.h"
#include <pthread.h>

using namespace std;

struct EventComp 
{
	bool operator()(const pair<unsigned int, Event*> le, const pair<unsigned int, Event*> re) const {
		return (le.second->time() > re.second->time()) || (le.second->time()==re.second->time() && le.first>re.first);
	}
};

class EventQueue
{
public:
	EventQueue(void);
	~EventQueue(void);

	void pushStringEvent(const string &req, int worldFD = -1);
	inline pthread_mutex_t* mutex();
	inline time_t	topTime();
	inline Event*	popEvent();
	inline void		freeEvent(Event *e);
	inline Event*	allocateEvent();
	inline bool		isEmpty();
	inline void		safePushEvent(Event *e);
protected:
	inline void		acquireLock();
	inline void		releaseLock();
	inline void		pushEvent(Event *e);

	typedef priority_queue<pair<unsigned int, Event*>, vector<pair<unsigned int, Event*> >, EventComp> PriorityEventQueue;
	PriorityEventQueue	event_queue_;
	pthread_mutex_t		mutex_;
	unsigned int		inner_counter_;
	static const string delims_;
	log4cxx::LoggerPtr	logger_;
};

////////////////////////inline function////////////////////////////
inline pthread_mutex_t* EventQueue::mutex() 
{
	return &mutex_;
}

inline time_t EventQueue::topTime() 
{
	return event_queue_.top().second->time();
}

inline Event* EventQueue::popEvent() 
{
	Event *e = event_queue_.top().second;
	event_queue_.pop();
	return e;
}

inline void EventQueue::freeEvent(Event *e)	
{
	delete e;
}

inline Event* EventQueue::allocateEvent() 
{
	return new Event();
}

inline bool EventQueue::isEmpty() 
{
	return event_queue_.empty();
}

inline void EventQueue::safePushEvent(Event *e) 
{
	acquireLock();
	pushEvent(e);
	releaseLock();		
}

inline void EventQueue::acquireLock() 
{
	pthread_mutex_lock(&mutex_);
}

inline void EventQueue::releaseLock() 
{
	pthread_mutex_unlock(&mutex_);
}

inline void EventQueue::pushEvent(Event *e) 
{
	if (!e->IsInitialized()) {
		LOG4CXX_ERROR (logger_, "Event: "<<e->cmd()<<" is not initialized! in pushEvent\n"<<e->DebugString());
	} else {
		event_queue_.push(make_pair(inner_counter_++, e));
	}
}

#endif
