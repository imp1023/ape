#pragma once
#ifndef CLOCK_CLASS
#define CLOCK_CLASS
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
#include "../common/const_def.h"

class Clock
{
public:
    Clock(void);
    ~Clock(void);

    void refresh();

    inline time_t now()
    {
        return now_;
    }

    inline time_t time()
    {
        return (time_t) (last_refresh_sys_ / 1000000);
    }

    inline time_t refreshNow()
    {
        refresh();
        return now_;
    }

    float ratio()
    {
        return ratio_;
    }

    void setRatio(float ratio)
    {
        ratio_ = ratio;
    }
    void setNow(time_t now);

    // The unit of time is milliseconds
    static time_t getCurrentSystemTime();
    static void sleep(Time_val &timeout);
    static Clock* GetInst();
    static Clock* CreatInst();
	static time_t getUTime();

public:
    static int64 GetSecond();
    static int64 GetMinute();
    static int64 GetHour();
    static int64 GetDay();
    static bool  IsToday(int64 llSecond);
protected:
    // The unit of time is milliseconds
    time_t now_;
    time_t last_refresh_sys_;

    float ratio_;

    static Clock* m_pInst;
public:
    const static int64  llDaySecond  = 60 * 60 * 24;
    const static int64  llHourSecnod = 60 * 60;
} ;

inline Clock* Clock::GetInst()
{
    return m_pInst;
}

inline Clock* Clock::CreatInst()
{
    if (m_pInst == NULL)
    {
        m_pInst = new Clock();
        return m_pInst;
    }
    return m_pInst;
}

inline int64 Clock::GetSecond()
{
    return GetInst()->now_ / 1000;
}

inline int64 Clock::GetMinute()
{
    return GetInst()->now_ / 1000 / 60;
}

inline int64 Clock::GetHour()
{
    return GetInst()->now_ / 1000 / 60 / 60;
}

inline int64 Clock::GetDay()
{
    return (GetHour()+8) / 24;
}


#endif
