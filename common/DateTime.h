#ifndef _DATE_H_
#define _DATE_H_

#include <string>
using std::string;
#include "Timestamp.h"
#include "Clock.h"

class DateTime
{
public:
    enum Months
    {
        JANUARY = 1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

    enum DaysOfWeek
    {
        SUNDAY = 0,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY
    };

    enum TimeSpan
    {
        SecondsOfMinute = 60,
        SecondsOfHour   = 60*60,
        SecondsOfDay    = 24*60*60,
    };

    DateTime();
        /// Creates a DateTime for the current date and time.

    DateTime(time_d td);

    DateTime(const Timestamp& timestamp);
        /// Creates a DateTime for the date and time given in
        /// a Timestamp.

    DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);
        /// Creates a DateTime for the given Gregorian date and time.
        ///   * year is from 1901 to 2038.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 59.

    DateTime(const DateTime& datetime);
        /// Copy constructor. Creates the DateTime from another one.

    ~DateTime();

    DateTime& operator = (const DateTime& dateTime);
    DateTime& operator = (const Timestamp& timestamp);

    DateTime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);
        /// Assigns a Gregorian date and time.
        ///   * year is from 1901 to 2038.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 59.

    DateTime& assign_quick(int year_month_day, int hour = 0, int minute = 0, int second = 0);
        /// Assigns a Gregorian date and time.
        ///   * year_month_day is like 20130101.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 59.

    DateTime& assign_str(const string& str, const string& fmt);
    DateTime& assign_str(const string& str);

    int year() const;
        /// Returns the year.

    int month() const;
        /// Returns the month (1 to 12).

    int weekOfYear(int firstDayOfWeek = MONDAY) const;
        /// Returns the week number within the year.
        /// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
        /// The returned week number will be from 0 to 53. Week number 1 is the week 
        /// containing January 4. This is in accordance to ISO 8601.
        /// 
        /// The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
        /// on a Saturday, week 1 will be the week starting on Monday, January 3.
        /// January 1 and 2 will fall within week 0 (or the last week of the previous year).
        ///
        /// For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1.
        /// There will be no week 0 in 2007.

    int day() const;
        /// Returns the day witin the month (1 to 31).

    int dayOfWeek() const;
        /// Returns the weekday (0 to 6, where
        /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).

    int dayOfYear() const;
        /// Returns the number of the day in the year.
        /// January 1 is 1, February 1 is 32, etc.

    int hour() const;
        /// Returns the hour (0 to 23).

    int hourAMPM() const;
        /// Returns the hour (0 to 12).

    bool isAM() const;
        /// Returns true if hour < 12;

    bool isPM() const;
        /// Returns true if hour >= 12.

    int minute() const;
        /// Returns the minute (0 to 59).

    int second() const;
        /// Returns the second (0 to 59).

    int julianDate() const;

    Timestamp timestamp() const;
        /// Returns the date and time expressed as a Timestamp.

    time_d stdtime() const;

    int year_month_day() const;
        ///   * year_month_day is like 20130101.

    int year_week(int firstDayOfWeek = MONDAY) const;
        ///   * year_week is like 201301.

    bool operator == (const DateTime& dateTime) const;	
    bool operator != (const DateTime& dateTime) const;	
    bool operator <  (const DateTime& dateTime) const;	
    bool operator <= (const DateTime& dateTime) const;	
    bool operator >  (const DateTime& dateTime) const;	
    bool operator >= (const DateTime& dateTime) const;	

    DateTime  operator +  (time_d span) const;
    DateTime  operator -  (time_d span) const;
    time_d    operator -  (const DateTime& dateTime) const;
    DateTime& operator += (time_d span);
    DateTime& operator -= (time_d span);

    static bool isLeapYear(int year);
        /// Returns true if the given year is a leap year;
        /// false otherwise.

    static int daysOfMonth(int year, int month);
        /// Returns the number of days in the given month
        /// and year. Month is from 1 to 12.

    static int daysOfYear(int year)
    {
        if (isLeapYear(year))
        {
            return 366;
        }
        return 365;
    }

    //////////////////////////////////////////////////////////////////////////
    //新加：判断是否为同一天
    //返回：true同一天，false不同一天
    static bool isSameDay(const DateTime& _rt1,const DateTime& _rt2)
    {
        if (_rt1.year() == _rt2.year())
        {
            if (_rt1.month() == _rt2.month())
            {
                if (_rt1.day() == _rt2.day())
                {
                    return true;
                }
            }
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //新加：判断是否为同一个月
    //返回：true同一月，false不同一月
    static bool isSameMonth(const DateTime& _rt1,const DateTime& _rt2)
    {
        if (_rt1.year() == _rt2.year())
        {
            if (_rt1.month() == _rt2.month())
            {
                return true;
            }
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //新加：判断是否为同一个周
    //返回：true同一周，false不同一周
    static bool isSameWeek(const DateTime& _rt1,const DateTime& _rt2)
    {
        if (_rt1.year() == _rt2.year())
        {
            if (_rt1.weekOfYear() == _rt2.weekOfYear())
            {
                return true;
            }
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //新加：判断两个时间之过了多少天(经过几次24点)
    //返回：天数
    static int pastDay(const DateTime& _rt1, const DateTime& _rt2)
    {
        int day1 = Clock::getLocalDay((int)_rt1.stdtime());
        int day2 = Clock::getLocalDay((int)_rt2.stdtime());
        return day1 > day2 ? (day1 - day2) : (day2 - day1);
    }

    static int pastDay(const int64 _t1, const int64 _t2)
    {
        int day1 = Clock::getLocalDay((int)_t1);
        int day2 = Clock::getLocalDay((int)_t2);
        return day1 > day2 ? (day1 - day2) : (day2 - day1);
    }

protected:

    void computeDaytime();
    void computeStdtime();

private:

    void carryDigit(short& current_digit, short& higher_digit, short minVal, short maxVal);
    void normalize();

    time_d          td_;
    short           year_;
    short           month_;
    short           day_;
    short           hour_;
    short           minute_;
    short           second_;
};

inline Timestamp DateTime::timestamp() const
{
    return Timestamp(td_);
}

inline int DateTime::year() const
{
    return year_;
}

inline int DateTime::month() const
{
    return month_;
}

inline int DateTime::day() const
{
    return day_;
}

inline int DateTime::hour() const
{
    return hour_;
}

inline int DateTime::hourAMPM() const
{
    if (hour_ < 1)
        return 12;
    else if (hour_ > 12)
        return hour_ - 12;
    else
        return hour_;
}


inline bool DateTime::isAM() const
{
    return hour_ < 12;
}


inline bool DateTime::isPM() const
{
    return hour_ >= 12;
}

inline int DateTime::minute() const
{
    return minute_;
}


inline int DateTime::second() const
{
    return second_;
}

inline time_d DateTime::stdtime() const
{
    return td_;
}

inline int DateTime::year_month_day() const
{
    return year_ * 10000 + month_ * 100 + day_;
}

inline int DateTime::year_week(int firstDayOfWeek) const
{
    return year_ * 100 + weekOfYear(firstDayOfWeek);
}

inline int DateTime::julianDate() const
{
    return day_ - 32075L +
        1461L * ( year_ + 4800L + ( month_ - 14L ) / 12L ) / 4L +
        367L * ( month_ - 2L - ( month_ - 14L ) / 12L * 12L ) / 12L -
        3L * ( ( year_ + 4900L + ( month_ - 14L ) / 12L ) / 100L ) / 4L;
}

inline bool DateTime::operator == (const DateTime& dateTime) const
{
    return td_ == dateTime.td_;
}


inline bool DateTime::operator != (const DateTime& dateTime) const
{
    return td_ != dateTime.td_;
}


inline bool DateTime::operator < (const DateTime& dateTime) const
{
    return td_ < dateTime.td_;
}


inline bool DateTime::operator <= (const DateTime& dateTime) const
{
    return td_ <= dateTime.td_;
}


inline bool DateTime::operator > (const DateTime& dateTime) const
{
    return td_ > dateTime.td_;
}


inline bool DateTime::operator >= (const DateTime& dateTime) const
{
    return td_ >= dateTime.td_;
}

inline DateTime DateTime::operator + (time_d d) const
{
    return DateTime(stdtime() + d);
}

inline DateTime DateTime::operator - (time_d d) const
{
    return DateTime(stdtime() - d);
}

inline time_d DateTime::operator - (const DateTime& datetime) const
{
    return stdtime() - datetime.stdtime();
}

inline DateTime& DateTime::operator += (time_d d)
{
    td_ += d;
    computeDaytime();
    return *this;
}

inline DateTime& DateTime::operator -= (time_d d)
{
    td_ -= d;
    computeDaytime();
    return *this;
}

inline bool DateTime::isLeapYear(int year)
{
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}


#endif

