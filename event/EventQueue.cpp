#include "EventQueue.h"
#include "../common/string-util.h"

const string EventQueue::delims_ = ",";

EventQueue::EventQueue() 
{
	logger_ = log4cxx::Logger::getLogger("EventQueue");
	pthread_mutex_init(&mutex_, NULL);
	inner_counter_ = 0;
}

EventQueue::~EventQueue(void)
{
	pthread_mutex_destroy(&mutex_);
	while (!event_queue_.empty())
	{
		Event *e = event_queue_.top().second;
		delete e;
		event_queue_.pop();
	}
}

//////////////////////////////////////////////////////////////////////////
// general 
//////////////////////////////////////////////////////////////////////////
void EventQueue::pushStringEvent(const string &req, int worldFD /* = -1  */) 
{
	Event *e = allocateEvent();
	if (e->ParseFromString(req)) 
	{
		//if(worldFD >= 0)
		//{
		//	e->set_fromworld_fd( worldFD ); 
		//}
		safePushEvent(e);
	}
	else 
	{
		LOG4CXX_ERROR(logger_, "Parse event error: \n"<<req);
		freeEvent(e);
	}
}
