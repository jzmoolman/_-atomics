
#ifndef _MCSLOCK_H_
#define _MCSLOCK_H_

#include <atomic>

struct qnode_t {
    std::atomic<void*> next;
    std::atomic<bool> wait;
};

class MCSLock  {
    std::atomic<void*> tail;
    public:
        MCSLock() { tail.store(NULL, std::memory_order_relaxed);}
        void acquire(qnode_t* p, int id); 
        void release(qnode_t* p, int id);
};

#endif