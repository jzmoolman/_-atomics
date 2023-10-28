#include <iostream>
#include "../include/mcslock.h"
#include "../include/zutils.h"

/* 
 * Private
 */

void MCSLock::_strong_acquire(qnode_t* p, int tid) {
    // std::cout <<  "id " << tid << " p value " <<  p << std::endl << std::flush;
    p->next.store(NULL, std::memory_order_relaxed);
    p->wait.store(true, std::memory_order_relaxed);
    qnode_t* prev;
    while(true) {
        prev = (qnode_t*)tail.load(std::memory_order_seq_cst);
        if (cas(tail,(void*)prev , (void*)p, std::memory_order_seq_cst) ){
        //    std::cout <<  "lock tail" <<  prev << std::endl;
           break;
        } 
    }

    if (prev != NULL) {  // queue is not empty
        // std::cout << "(" <<tid<< ")" << "before spin" << std::endl;
        prev->next.store(p,std::memory_order_seq_cst);
        while ( p->wait.load(std::memory_order_seq_cst)) {}
        // std::cout << "(" <<tid<< ")" << "after spin" << std::endl;
    }
} 

void MCSLock::_strong_release(qnode_t* p, int id) {
    // std::cout << "(" << id << ")" << "release"  << std::endl;
    if (cas(tail, (void*)p, (void*)NULL, std::memory_order_seq_cst)){
        // std::cout << "(" << id << ")" << " tail is null" << std::endl;
        // last in the queue
        // nothing to do
        return;
    } 

    qnode_t* succ;
    while( true ) {
        succ = (qnode_t*)(p->next.load(std::memory_order_seq_cst));
        if (succ != NULL) break;
    }   
    succ->wait.store(false,std::memory_order_seq_cst);
}

void MCSLock::_weak_acquire(qnode_t* p, int tid) {
    p->next.store(NULL, std::memory_order_relaxed);
    p->wait.store(true, std::memory_order_relaxed);
    qnode_t* prev;
    while(true) {
        prev = (qnode_t*)tail.load(std::memory_order_acquire);
        if (cas(tail,(void*)prev , (void*)p, std::memory_order_acq_rel) ){
           break;
        } 
    }

    if (prev != NULL) {  // queue is not empty
        prev->next.store(p,std::memory_order_release);
        while ( p->wait.load(std::memory_order_acquire)) {}
    }
} 

void MCSLock::_weak_release(qnode_t* p, int id) {
    if (cas(tail, (void*)p, (void*)NULL, std::memory_order_acq_rel)){
        // last in the queue
        // nothing to do
        return;
    } 

    qnode_t* succ;
    while( true ) {
        succ = (qnode_t*)(p->next.load(std::memory_order_acquire));
        if (succ != NULL) break;
    }   
    succ->wait.store(false,std::memory_order_release);
}

/* 
 * Public
 */

void MCSLock::acquire(qnode_t* p, int tid) {
    if (_order == lo_strong )
        _strong_acquire(p, tid);
    else if (_order == lo_weak)
        _weak_acquire(p, tid);
}

void MCSLock::release(qnode_t* p, int tid) {
    if (_order == lo_strong )
        _strong_release(p, tid);
    else if (_order == lo_weak)
        _weak_release(p, tid);
}