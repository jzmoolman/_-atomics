#include <iostream>
#include "../include/petersonrellock.h"

PetersonrelLock::PetersonrelLock() {
    _desires[0].store(false, std::memory_order_relaxed);
    _desires[1].store(false, std::memory_order_relaxed);
    _turn.store(0, std::memory_order_release);
}

void PetersonrelLock::lock(int tid) {
    int other = 1 - tid; //(tid+1)%2;  if 1 - > 0 if 0  -> 1
    _desires[tid].store(true, std::memory_order_release);
    _turn.store(other, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    while( _desires[other].load(std::memory_order_acquire) && 
      _turn.load(std::memory_order_acquire ) == other ) {}
};

void PetersonrelLock::unlock(int tid) {
    _desires[tid].store(false,std::memory_order_release);
}