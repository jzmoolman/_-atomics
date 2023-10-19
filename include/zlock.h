
#ifndef _ZLOCK_H_
#define _ZLOCK_H_

class ZLock {
    public :
        ZLock() {};
        void virtual lock() = 0; 
        void virtual unlock() = 0;
};

#endif