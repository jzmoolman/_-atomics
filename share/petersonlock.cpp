#include <iostream>
#include "../include/petersonlock.h"

PetersonLock::PetersonLock() {
    _desires[0].store(false, std::memory_order_relaxed);
    _desires[1].store(false, std::memory_order_relaxed);
    _turn.store(0, std::memory_order_seq_cst);
}

void PetersonLock::lock(int tid) {
    int other = (tid+1)%2;
    _desires[tid].store(true, std::memory_order_seq_cst);
    _turn.store(other, std::memory_order_seq_cst);
    while( _desires[other].load(std::memory_order_seq_cst) && 
      _turn.load(std::memory_order_seq_cst) == other ) {}
};

void PetersonLock::unlock(int tid) {
    _desires[tid].store(false,std::memory_order_seq_cst);
};