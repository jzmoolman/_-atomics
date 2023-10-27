
#include "../include/ttaslock.h"
#include "../include/zutils.h"

/*
 *  Private
 */

 void TTASLock::_strong_lock() {
    //use short circuit
    while (flag.load(std::memory_order_seq_cst) || !tas(flag, std::memory_order_seq_cst)) {};
 }   

void TTASLock::_strong_unlock() {
    flag.store(false, std::memory_order_seq_cst); // Release the lock
}

 void TTASLock::_weak_lock() {
    while (flag.load(std::memory_order_relaxed) == true || tas(flag, std::memory_order_acq_rel) == false) {};
 }   

void TTASLock::_weak_unlock() {
    flag.store(false, std::memory_order_release); // Release the lock
    // std::atomic_thread_fence(std::memory_order_seq_cst);
}

/*
 *  Public
 */

void TTASLock::lock() {
    if (_order == lo_strong)
        _strong_lock();
    else if (_order == lo_weak) 
        _weak_lock();
    else 
        _strong_lock();

}
void TTASLock::unlock() {
    if (_order == lo_strong)
        _strong_unlock();
    else if (_order == lo_weak) 
        _weak_unlock();
    else 
        _strong_unlock();

}
