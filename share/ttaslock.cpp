
#include "../include/ttaslock.h"
#include "../include/zutils.h"

void TTASLock::lock() {
    
    //use short circuit
    // std::memory_order order = std::memory_order_acquire;
    std::memory_order order = std::memory_order_acq_rel;
    // std::memory_order order = std::std::memory_order_seq_cst;
    while (flag.load(std::memory_order_relaxed) || !tas(flag, order)) {};
}

void TTASLock::unlock() {
    flag.store(false, std::memory_order_release); // Release the lock
}


