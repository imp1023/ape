#ifndef __MAGIC_QUEUE_H__
#define __MAGIC_QUEUE_H__

#include <cstddef>


//一个简易的队列模仿，快速，不用排序，根据插入的时间顺序有序，不会产生内存碎片，只能添加，不支持删除

template<class T,int maxsize = 10>
class magicqueue 
{
private:
    T m_queue[maxsize];
    const int m_size;
    int m_current;  //当前的队列头
    int m_count;     //插入队列的总数
public:
    magicqueue():m_size(maxsize)
    {
        m_current = 0;
        m_count = 0;
    }

    ~magicqueue() {}

    void Push(T& _rt);

//     const int FillSortArray(T _p[],int _c);

    const int FillSortArray(T* const _p, int c);

    const int GetCurrentIndex() {return m_current;}
    const int GetTotalPushCount() {return m_count;}
protected:
    const int FillArray(T _p[], int _c);
};

template<class T,int maxsize>
void magicqueue<T,maxsize>::Push(T& _rt)
{
    m_current = m_current % m_size;
    m_queue[m_current] = _rt;
    m_current++;
    if (m_count < m_size)      //防止溢出
    {
        m_count++;
    }
}

// template<class T,int maxsize>
// const int magicqueue<T,maxsize>::FillSortArray(T _p[],int _c)
// {
//     if (_p == NULL)
//     {
//         return 0;
//     }
// 
//     if (m_count == 0)
//     {
//         return 0;
//     }
// 
//     //三个数(_c,m_size,m_count)中取最小
//     int tmp = _c > m_size ? m_size : _c;
//     tmp = tmp > m_count ? m_count : tmp;
//     
//     return this->FillArray(_p,tmp);
// }

template<class T,int maxsize>
const int magicqueue<T,maxsize>::FillSortArray(T* const _p,int _c)
{
    if (_p == NULL)
    {
        return 0;
    }

    if (m_count == 0)
    {
        return 0;
    }

    //三个数(_c,m_size,m_count)中取最小
    int tmp = _c > m_size ? m_size : _c;
    tmp = tmp > m_count ? m_count : tmp;

    return this->FillArray(_p,tmp);
}

template<class T,int maxsize>
const int magicqueue<T,maxsize>::FillArray(T _p[], int _c)
{
    if (_p == NULL)
    {
        return 0;
    }
    if (_c == 0)
    {
        return 0;
    }

    int offset = (m_current - 1 + m_size) % m_size;
    int i = 0;
    for (; i < _c; i++)
    {
        _p[i] = m_queue[offset];
        offset--;
        if (offset < 0)
        {
            offset = m_size - 1;
        }
    }
    return i;
}


#endif

