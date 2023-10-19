
#ifndef _TICKETLOCK_H_
#define _TICKETLOCK_H_

#include <atomic>
#include "zlock.h"

class TicketLock : public ZLock {
    std::atomic<int> next_num = 0;
    std::atomic<int> now_serving = 0;
    public :
        TicketLock() {};
        void virtual lock(); 
        void virtual unlock();
};

#endif