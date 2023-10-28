#include <thread>
#include <iostream>

#include "../include/sensebarrier.h"

/*
 * Private
 */

void SenseBarrier::_strong_wait() {
    thread_local bool my_sense = 0;
    if(my_sense == 0) {
        my_sense = 1;         //flip my_sense
    } else {
       my_sense = 0;
    } 

    int my_cnt = fai(cnt,1, std::memory_order_seq_cst);
    /* last thread */   
    if (my_cnt == _num-1) {  
        cnt.store(0, std::memory_order_relaxed);
        sense.store(my_sense, std::memory_order_seq_cst);

    } else { 
    /* not last */
        while(sense.load(std::memory_order_seq_cst) != my_sense){}
    }
}

void SenseBarrier::_weak_wait() {
    thread_local bool my_sense = 0;
    if(my_sense == 0) {
        my_sense = 1;         //flip my_sense
    } else {
       my_sense = 0;
    } 

    int my_cnt = fai(cnt,1, std::memory_order_acq_rel);
    /* last thread */   
    if (my_cnt == _num-1) {  
        cnt.store(0, std::memory_order_relaxed);
        sense.store(my_sense, std::memory_order_release);

    } else { 
    /* not last */
        while(sense.load(std::memory_order_acquire) != my_sense){}
    }
}
/*
 * Public
 */

void SenseBarrier::wait() {
    if (_order == lo_strong)
        _strong_wait();
    else 
        _weak_wait();
}