#include <iostream>
#include <thread>
#include <assert.h>

using namespace std;

// #define DEBUG_ENABLED

struct DummyStruct { int n;};
int m;
atomic<DummyStruct *> p;

void consumeFunc() {
    auto DStr = new DummyStruct;
    DStr->n = 13;
    m = 11;
    p.store(DStr, memory_order_release);
}

void releaseFunc() {
    DummyStruct *DStr;
    while( !(DStr = p.load(memory_order_consume))) {};
    assert(p.load(memory_order_relaxed)->n == 13);
    assert(m == 11); // data race
}


int main(int argc, char** argv) {
    cout << "Atomic test program" << endl;
    while ( true ) {
        p = NULL;
        m = 0;
        thread T1(consumeFunc);
        thread T2(releaseFunc);
    
        T1.join();
        T2.join();
    }

} 