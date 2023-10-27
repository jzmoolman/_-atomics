#ifndef _TTASLOCK_H_
#define _TTASLOCK_H_

#include <atomic>

#include "zlock.h"

class TTASLock : public ZLock {
    private:
        int _order;
        void _strong_lock();
        void _strong_unlock();
        void _weak_lock();
        void _weak_unlock();
        std::atomic<bool> flag;
    public :
        TTASLock(int order): flag(false), _order(order) {};
        void lock(); 
        void unlock();
};

#endif