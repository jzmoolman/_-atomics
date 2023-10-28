
#ifndef _SENSEBARRIER_H_
#define _SENSEBARRIER_H_

#include <atomic>
#include "zutils.h"
#include "zbarrier.h"

class SenseBarrier : public ZBarrier {
    private:
        std::atomic<int> cnt=0;
        std::atomic<int> sense=0;
        int _order;
        int _num;
        void _strong_wait();
        void _weak_wait();
    public:
        SenseBarrier(int order,int num): _order(order), _num(num)  { }; 
        void wait();
};

#endif