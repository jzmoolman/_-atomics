#ifndef _TASLOCK_H_
#define _TASLOCK_H_

#include <atomic>
#include "zlock.h"

class TASLock : public ZLock {
    std::atomic_flag flag;
    public :
        TASLock(): flag(false) {};

        void lock(); 
        void unlock();
};

#endif