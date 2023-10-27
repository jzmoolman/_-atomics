#ifndef _PTHREADLOCK_H_
#define _PTHREADLOCK_H_

#include <thread>
#include "zlock.h"

class PThreadLock : public ZLock {
    std::mutex _mutex;
    public :
        void lock(); 
        void unlock();
};

void PThreadLock::lock() {
    _mutex.lock();
}

void PThreadLock::unlock() {
    _mutex.unlock();
}

#endif