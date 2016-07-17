#ifndef _TIME_STAMP_H_
#define _TIME_STAMP_H_
#pragma once

#include "const_def.h"
#include <time.h>

class Timestamp
{
public:
    Timestamp();
    Timestamp(time_d td);
    Timestamp(const Timestamp& other);
    ~Timestamp();

    Timestamp& operator = (const Timestamp& other);
    Timestamp& operator = (time_d td);

    void update();

    bool operator == (const Timestamp& ts) const;
    bool operator != (const Timestamp& ts) const;
    bool operator >  (const Timestamp& ts) const;
    bool operator >= (const Timestamp& ts) const;
    bool operator <  (const Timestamp& ts) const;
    bool operator <= (const Timestamp& ts) const;

    Timestamp  operator +  (time_d d) const;
    Timestamp  operator -  (time_d d) const;
    time_d     operator -  (const Timestamp& ts) const;
    Timestamp& operator += (time_d d);
    Timestamp& operator -= (time_d d);

    time_d stdtime() const;
    int stdtime_int32() const; // 2038ÄêÊ§Ð§
    time_d elapsed() const;
    bool isElapsed(time_d time_span = 0) const;

#ifdef _TEST

    static void setTime(time_d t);

#endif

private:
    time_d stamp_;
};



inline bool Timestamp::operator == (const Timestamp& ts) const
{
    return stamp_ == ts.stamp_;
}


inline bool Timestamp::operator != (const Timestamp& ts) const
{
    return stamp_ != ts.stamp_;
}


inline bool Timestamp::operator >  (const Timestamp& ts) const
{
    return stamp_ > ts.stamp_;
}


inline bool Timestamp::operator >= (const Timestamp& ts) const
{
    return stamp_ >= ts.stamp_;
}


inline bool Timestamp::operator <  (const Timestamp& ts) const
{
    return stamp_ < ts.stamp_;
}


inline bool Timestamp::operator <= (const Timestamp& ts) const
{
    return stamp_ <= ts.stamp_;
}


inline Timestamp Timestamp::operator + (time_d d) const
{
    return Timestamp(stamp_ + d);
}


inline Timestamp Timestamp::operator - (time_d d) const
{
    return Timestamp(stamp_ - d);
}


inline time_d Timestamp::operator - (const Timestamp& ts) const
{
    return stamp_ - ts.stamp_;
}


inline Timestamp& Timestamp::operator += (time_d d)
{
    stamp_ += d;
    return *this;
}


inline Timestamp& Timestamp::operator -= (time_d d)
{
    stamp_ -= d;
    return *this;
}

#ifndef _TEST

inline void Timestamp::update()
{
    stamp_ = time(NULL);
}

#endif

inline time_d Timestamp::stdtime() const
{
    return stamp_;
}

inline int Timestamp::stdtime_int32() const
{
    return static_cast<int>(stamp_);
}

inline time_d Timestamp::elapsed() const
{
    Timestamp now;
    return now - *this;
}

inline bool Timestamp::isElapsed(time_d time_span) const
{
    Timestamp now;
    time_d diff = now - *this;
    return diff >= time_span;
}

#endif


