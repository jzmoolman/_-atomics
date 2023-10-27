#ifndef _PTHREADBARRIER_H_
#define _PTHREADBARRIER_H_

#include <barrier>
#include "../include/zbarrier.h"

class PThreadBarrier : public ZBarrier {
    protected:
        std::barrier<>* _bar;
    public :
        PThreadBarrier(int cnt) {
            _bar = new std::barrier(cnt);
        }; 
        void wait(); 
};

void PThreadBarrier::wait() {
    _bar->arrive_and_wait();
}

#endif


