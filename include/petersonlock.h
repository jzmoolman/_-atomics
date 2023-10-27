#ifndef _PETERSONLOCK_H_
#define _PETERSONLOCK_H_

#include <atomic>

class PetersonLock {
    private:
        std::atomic<bool> _desires[2];
        std::atomic<int> _turn;
    public: 
        PetersonLock();
        void lock(int tid);
        void unlock(int tid);
    
};

#endif