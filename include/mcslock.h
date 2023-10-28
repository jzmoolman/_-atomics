
#ifndef _MCSLOCK_H_
#define _MCSLOCK_H_

#include <atomic>
#include "zutils.h"

struct qnode_t {
    std::atomic<void*> next;
    std::atomic<bool> wait;
};

class MCSLock  {
    private:
        std::atomic<void*> tail;
        int _order;
        void _strong_acquire(qnode_t* p, int id); 
        void _strong_release(qnode_t* p, int id);
        void _weak_acquire(qnode_t* p, int id); 
        void _weak_release(qnode_t* p, int id);

    public:
        MCSLock(int order): _order(order) { tail.store(NULL, std::memory_order_relaxed);}
        void acquire(qnode_t* p, int id); 
        void release(qnode_t* p, int id);
};

#endif