#ifndef _PETERSONRELLOCK_H_
#define _PETERSONRELLOCK_H_

#include <atomic>

class PetersonrelLock {
    private:
        std::atomic<bool> _desires[2];
        std::atomic<int> _turn;
    public: 
        PetersonrelLock();
        void lock(int tid);
        void unlock(int tid);
    
};

#endif