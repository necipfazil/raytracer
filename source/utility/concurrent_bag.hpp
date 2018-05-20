#ifndef __CONCURRENT_BAG__
#define __CONCURRENT_BAG__

#include <forward_list>
#include <mutex>

template<class T>
class ConcurrentBag
{
    private:
        std::forward_list<T> bag;
        std::mutex bagMutex;
    public:
        ConcurrentBag(const std::forward_list<T> & bag)
            : bag(bag) { }
        
        // returns true if there exists an element to be popped and fills tOut
        // otherwise, returns false and does not mutate tOut
        bool pop(T & tOut);
        
        void push(const T & t);
};

#include "concurrent_bag_impl.hpp"

#endif
