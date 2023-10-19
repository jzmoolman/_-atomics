#include <iostream>
#include "../include/mcslock.h"
#include "../include/zutils.h"


// void MCSLock::acquire(qnode_t* p) {
//     p->next.store(NULL, std::memory_order_relaxed);
//     p->wait.store(true, std::memory_order_relaxed);
    
//     qnode_t* prev;
//     prev = (qnode_t*)vcas(tail, (void*)tail.load(std::memory_order_seq_cst), (void*)p, std::memory_order_seq_cst);
//     if (prev != NULL) {  // queue is not empty
//         prev->next.store(p,std::memory_order_seq_cst);
//         while ( p->wait.load(std::memory_order_seq_cst)) {}
//     }
// } 


// void MCSLock::release(qnode_t* p) {
//     qnode_t* succ = (qnode_t*)p->next.load(std::memory_order_relaxed);
//     if (succ == NULL) {
//         if (cas(tail, (void*)p, (void*)NULL, std::memory_order_seq_cst)) 
//             return;
//         // if above fails then we aren't the last anymore
//         while( true ) {
//            succ = (qnode_t*)(p->next.load(std::memory_order_seq_cst));
//            if (succ != NULL) break;
//         }   
//     }
//     succ->wait.store(false);
// }

void MCSLock::acquire(qnode_t* p, int tid) {
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

void MCSLock::release(qnode_t* p, int id) {
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
