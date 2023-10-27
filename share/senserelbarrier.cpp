#include <thread>

#include <iostream>
#include "../include/senserelbarrier.h"
#include "../include/zutils.h"


void SenserelBarrier::wait() {
    thread_local bool my_sense = 0;
    if(my_sense == 0) {
        my_sense = 1;         //flip my_sense
    } else {
       my_sense = 0;
    } 

    // std::cout << "Sense " << sense.load(std::memory_order_relaxed) << " " << my_sense << std::endl;
    int my_cnt = fai(cnt,1, std::memory_order_seq_cst);
    // std::cout << "my_count " << my_cnt << std::endl; 
    // std::cout << "count " << _num << std::endl; 
    /* last thread */   
    if (my_cnt == _num-1) {  
        cnt.store(0, std::memory_order_relaxed);
        sense.store(my_sense, std::memory_order_seq_cst);

    } else { 
    /* not last */
        while(sense.load(std::memory_order_seq_cst) != my_sense){}
    }
}