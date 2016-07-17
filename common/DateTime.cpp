#include "DateTime.h"
#include <pthread.h>
#include "ServerConfig.h"
#include "string-util.h"

DateTime::DateTime()
{
    Timestamp now;
    td_ = now.stdtime();
    computeDaytime();
}

DateTime::DateTime(const Timestamp& timestamp)
    : td_(timestamp.stdtime())
{                             
    computeDaytime();
}

DateTime::DateTime(time_d td)
    : td_(td)
{
    computeDaytime();
}

DateTime::DateTime(int year, int month, int day, int hour /*= 0*/, int minute /*= 0*/, int second /*= 0*/)
{
    assign(year, month, day, hour, minute, second);
}

DateTime::DateTime(const DateTime& datetime):
    td_(datetime.td_),
    year_(datetime.year_),
    month_(datetime.month_),
    hour_(datetime.hour_),
    minute_(datetime.minute_),
    second_(datetime.second_)
{

}

DateTime::~DateTime()
{

}

DateTime& DateTime::operator = (const DateTime& datetime)
{
    if (&datetime != this)
    {
        td_ = datetime.td_;
        year_ = datetime.year_;
        month_ = datetime.month_;
        day_ = datetime.day_;
        hour_ = datetime.hour_;
        minute_ = datetime.minute_;
        second_ = datetime.second_;
    }
    return *this;
}

DateTime& DateTime::operator = (const Timestamp& timestamp)
{
    td_ = timestamp.stdtime();
    computeDaytime();
    return *this;
}

int DateTime::weekOfYear(int firstDayOfWeek) const
{
    /// find the first firstDayOfWeek.
    int baseDay = 1;
    while (DateTime(year_, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

    int doy  = dayOfYear();
    int offs = baseDay <= 4 ? 0 : 1; 
    if (doy < baseDay)
        return offs;
    else
        return (doy - baseDay)/7 + 1 + offs;
}

int DateTime::dayOfWeek() const
{
    return (julianDate()+1)%7;
}

int DateTime::dayOfYear() const
{
    int doy = 0;
    for (int month = 1; month < month_; ++month)
        doy += daysOfMonth(year_, month);
    doy += day_;
    return doy;
}

int DateTime::daysOfMonth(int year, int month)
{
    if (month < 1 || month > 12)
        return 30;

    static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;
    else
        return daysOfMonthTable[month];
}

void DateTime::computeDaytime()
{
    tm m;
    time_t tt = static_cast<time_t>(td_);
    localtime_r(&tt, &m);
    
    year_ = m.tm_year + 1900;
    month_ = m.tm_mon + 1;
    day_ = m.tm_mday;
    hour_ = m.tm_hour;
    minute_ = m.tm_min;
    second_ = m.tm_sec;
}

void DateTime::computeStdtime()
{
    tm m;
    m.tm_year = year_ - 1900;
    m.tm_mon = month_ - 1;
    m.tm_mday = day_;
    m.tm_hour = hour_;
    m.tm_min = minute_;
    m.tm_sec = second_;
    m.tm_isdst = false;

    td_ = mktime(&m);
}

DateTime& DateTime::assign(int year, int month, int day, int hour /* = 0 */, int minute /* = 0 */, int second /* = 0 */)
{
    year_ = year;
    month_ = month;
    day_ = day;
    hour_ = hour;
    minute_ = minute;
    second_ = second;

    normalize();
    computeStdtime();

    return *this;
}

DateTime& DateTime::assign_quick(int year_month_day, int hour /* = 0 */, int minute /* = 0 */, int second /* = 0 */)
{
    int day = year_month_day % 100;
    year_month_day /= 100;

    int month = year_month_day % 100;
    year_month_day /= 100;

    int year = year_month_day;

    assign(year, month, day, hour, minute, second);

    return *this;
}

void DateTime::carryDigit(short& current_digit, short& higher_digit, short minVal, short maxVal)
{

    if (current_digit > maxVal)
        higher_digit++;

    if (current_digit < minVal || current_digit > maxVal)
        current_digit = minVal;

}

void DateTime::normalize()
{                                      
    if (year_ < 69)
        year_ += 2000;
    else if (year_ < 100)
    {
        year_ += 1900;
    }

    carryDigit(second_, minute_, 0, 59);
    carryDigit(minute_, hour_, 0, 59);
    carryDigit(hour_, day_, 0, 23);

    carryDigit(month_, year_, 1, 12);
    carryDigit(day_, month_, 1, daysOfMonth(year_, month_));
    carryDigit(month_, year_, 1, 12);
}

static const string default_time_format = "%Y-%m-%d %H:%M:%S";

#define SKIP_JUNK() \
    while (it != end && !isdigit(*it)) ++it


#define PARSE_NUMBER_N(var, n) \
{ int i = 0; while (i++ < n && it != end && isdigit(*it)) var = var*10 + ((*it++) - '0'); }

DateTime& DateTime::assign_str(const string& str, const string& fmt)
{
    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;

    std::string::const_iterator it   = str.begin();
    std::string::const_iterator end  = str.end();
    std::string::const_iterator itf  = fmt.begin();
    std::string::const_iterator endf = fmt.end();

    while (itf != endf && it != end)
    {
        if (*itf == '%')
        {
            if (++itf != endf)
            {
                switch (*itf)
                {
                case 'd':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(day, 2);
                    break;
                case 'm':
                case 'n':
                case 'o':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(month, 2);
                    break;					 
                case 'y':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(year, 2);
                    if (year >= 69) 
                        year += 1900;
                    else
                        year += 2000;
                    break;
                case 'Y':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(year, 4);
                    break;
                case 'H':
                case 'h':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(hour, 2);
                    break;
                case 'M':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(minute, 2);
                    break;
                case 'S':
                    SKIP_JUNK();
                    PARSE_NUMBER_N(second, 2);
                    break;
                }
                ++itf;
            }
        }
        else ++itf;
    }
    if (month == 0) month = 1;
    if (day == 0) day = 1;

    assign(year, month, day, hour, minute, second);

    return *this;
}

DateTime& DateTime::assign_str(const string& str)
{
    assign_str(str, default_time_format);
    return *this;
}
