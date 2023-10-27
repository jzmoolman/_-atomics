#ifndef _ZBARRIER_H_
#define _ZBARRIER_H_

class ZBarrier {
    public:
        ZBarrier() {};
        virtual void wait() = 0;
};

#endif


