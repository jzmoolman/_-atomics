#ifndef _TASLOCK_H_
#define _TASLOCK_H_

#include <atomic>
#include "zlock.h"

class TASLock : public ZLock {
    private:
        std::atomic_flag flag;
        int _order;
        void _strong_lock();
        void _strong_unlock();
        void _weak_lock();
        void _weak_unlock();
    public :
        TASLock(int order): flag(false), _order(order)  {};
        void lock(); 
        void unlock();
};

#endif