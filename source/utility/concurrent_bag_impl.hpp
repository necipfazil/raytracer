#ifndef __CONCURRENT_BAG_IMPL__
#define __CONCURRENT_BAG_IMPL__

#include <forward_list>
#include <mutex>

template<class T>
bool ConcurrentBag<T>::pop(T & tOut)
{
    std::lock_guard<std::mutex> guard(bagMutex);

    if(bag.empty())
    {
        return false;
    }
    else
    {
        tOut = bag.front();
        bag.pop_front();
        return true;
    }
}

template<class T>
void ConcurrentBag<T>::push(const T & t)
{
    std::lock_guard<std::mutex> guard(bagMutex);

    bag.push_front(t);
}

#endif