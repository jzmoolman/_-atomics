#include "../include/zutils.h"
#include "../include/taslock.h"

/*
 *  Private
 */

 void TASLock::_strong_lock() {
    while(flag.test_and_set(std::memory_order_seq_cst));
 }   

void TASLock::_strong_unlock() {
    flag.clear(std::memory_order_seq_cst);
}

 void TASLock::_weak_lock() {
    while(flag.test_and_set(std::memory_order_acquire));
 }   

void TASLock::_weak_unlock() {
    flag.clear(std::memory_order_release);
}


/*
 *  Public
 */

void TASLock::lock() {
    if (_order == lo_strong)
        _strong_lock();
    else if (_order == lo_weak) 
        _weak_lock();
    else 
        _strong_lock();

}
void TASLock::unlock() {
    if (_order == lo_strong)
        _strong_unlock();
    else if (_order == lo_weak) 
        _weak_unlock();
    else 
        _strong_unlock();

}