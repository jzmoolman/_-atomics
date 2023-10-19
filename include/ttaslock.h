#ifndef _TTASLOCK_H_
#define _TTASLOCK_H_

#include <atomic>

#include "zlock.h"

class TTASLock : public ZLock {
    std::atomic<bool> flag;
    public :
        TTASLock(): flag(false) {};

        void lock(); 
        void unlock();
};

#endif