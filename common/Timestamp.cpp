#include "Timestamp.h"

Timestamp::Timestamp()
{
    update();
}

Timestamp::Timestamp(time_d td)
{
    stamp_ = td;
}

Timestamp::Timestamp(const Timestamp& other)
{
    stamp_ = other.stamp_;
}

Timestamp::~Timestamp()
{

}

Timestamp& Timestamp::operator = (const Timestamp& other)
{
    stamp_ = other.stamp_;
    return *this;
}


Timestamp& Timestamp::operator = (time_d td)
{
    stamp_ = td;
    return *this;
}

#ifdef _TEST

static time_d test_time = 0;

void Timestamp::setTime(time_d t)
{
    test_time = t;
}

void Timestamp::update()
{
    if (test_time != 0)
        stamp_ = test_time;
    else
        stamp_ = time(NULL);
}

#endif

