
#ifndef _SENSERELBARRIER_H_
#define _SENSERELBARRIER_H_

#include <atomic>
#include "zbarrier.h"

class SenserelBarrier : public ZBarrier {
    std::atomic<int> cnt=0;
    std::atomic<int> sense=0;
    int _num;
    public:
        SenserelBarrier(int num)  { _num = num; } 
        void wait();
    
};

#endif