#ifndef _CLOCK_H_
#define _CLOCK_H_
#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include <sys/time.h>
#include <stdlib.h>
#endif

#ifdef _WIN32
typedef long long time_t;
typedef TIMEVAL Time_val;
#else
typedef timeval Time_val;
#endif

class Clock
{
private:
	Clock(void) {}
	~Clock(void) {}

public:
	// The unit of time is milliseconds
	static time_t getCurrentSystemTime();
	static void sleep(Time_val &timeout);
	static int getTimeZone();
	static int getLocalDay();
	static int getLocalDay(int second);
};

#endif
