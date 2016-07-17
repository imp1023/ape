#include "Clock.h"
#include <ctime>
#include <stdio.h>
#include <pthread.h>

time_t Clock::getCurrentSystemTime() {
#ifdef _WIN32
	FILETIME filetime;
	GetSystemTimeAsFileTime(&filetime);
	return ((time_t(filetime.dwHighDateTime - 0x019DB1DE)<<32) + 
		filetime.dwLowDateTime - 0xD53E8000)/10000;
#else
	timeval systime;
	gettimeofday(&systime, NULL);
	return (time_t(systime.tv_sec)*1000 + systime.tv_usec/1000);
#endif
}

void Clock::sleep(Time_val &timeout) {
#ifdef _WIN32
	int dummyfd = socket(PF_INET, SOCK_STREAM, 0);
	fd_set dummyread;
	FD_ZERO(&dummyread);
	FD_SET(dummyfd, &dummyread);
	select(dummyfd+1, &dummyread, NULL, NULL, &timeout);
#else
	select(1, NULL, NULL, NULL, &timeout);
#endif
}

int Clock::getTimeZone()
{
	time_t now = time(NULL);
	struct tm gm_ptr;
	struct tm local_ptr;
	gmtime_r(&now, &gm_ptr);
	localtime_r(&now, &local_ptr);
	
	int deta = local_ptr.tm_hour - gm_ptr.tm_hour;
	if (deta < -12)
		deta += 24;
	else if (deta > 12)
		deta -= 24;
	return deta;
}

int Clock::getLocalDay()
{
	return getLocalDay(int(time(NULL)));
}

int Clock::getLocalDay( int second )
{
	int now = second + Clock::getTimeZone() * 3600;
	return now / (24 * 3600);
}
