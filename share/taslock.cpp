#include "../include/taslock.h"

void TASLock::lock() {
    // while(flag.test_and_set(memory_order_seq_cst));  //
    // while(flag.test_and_set(memory_order_acq_rel));
    while(flag.test_and_set(std::memory_order_acquire));
}

void TASLock::unlock() {
    // flag.clear(memory_order_seq_cst);
    flag.clear(std::memory_order_release);
    // flag.clear(memory_order_release);
}