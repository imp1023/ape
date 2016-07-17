#ifndef _INDEXED_TIME_H_
#define _INDEXED_TIME_H_

#include "../common/const_def.h"
#include "../common/DateTime.h"

class IndexedTime
{
public:
    IndexedTime(Timestamp t, unsigned int index)
        : time_(t), index_(index)
    {}

    ~IndexedTime()
    {}

    inline Timestamp time() const { return time_; }
    inline void setTime(time_d val) { time_ = val; }
    inline unsigned int index() const { return index_; }
    inline void setIndex(unsigned int val) { index_ = val; }

private:

    Timestamp time_;
    unsigned int index_;
};

inline bool operator==(const IndexedTime& left, const IndexedTime& right)
{
    return left.time() == right.time() && left.index() == right.index();
}

inline bool operator!=(const IndexedTime& left, const IndexedTime& right)
{
    return !(left == right);
}

inline bool operator<(const IndexedTime& left, const IndexedTime& right)
{
    return left.time() < right.time() || left.time() == right.time() && left.index() < right.index();
}

inline bool operator>(const IndexedTime& left, const IndexedTime& right)
{
    return (right < left);
}

inline bool operator<=(const IndexedTime& left, const IndexedTime& right)
{
    return !(right < left);
}

inline bool operator>=(const IndexedTime& left, const IndexedTime& right)
{
    return !(left < right);
}



#endif

