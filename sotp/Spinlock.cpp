#include <iostream>
#include <thread>

using namespace std;

// #define DEBUG_ENABLED

class Spinlock {
    atomic_flag flag;
    public : 
        Spinlock(): flag(ATOMIC_FLAG_INIT) {}
        
        void lock() {
            // while(flag.test_and_set(memory_order_acquire));
            while(flag.test_and_set(memory_order_relaxed));
        }

        void unlock() {
            // flag.clear(memory_order_release);
            flag.clear(memory_order_relaxed);
        }
};

vector<int> SharedData;  

Spinlock lck;

void norace(int id) {
    while( SharedData[0] < 100000) {
        lck.lock();
        {
            SharedData[0]++;
            cout << SharedData[0] << endl;
        }
        lck.unlock();
    }
}

void race(int id) {
    while( SharedData[1] < 100000) {
        SharedData[1]++;
        lck.lock();
        cout << SharedData[1] << endl;
        lck.unlock();
        if ( id == 2)
            this_thread::sleep_for(chrono::microseconds(5));
    }
}


int main(int argc, char** argv) {
    cout << "Atomic test program" << endl;
    SharedData = {0,0};

    cout << "No race" << endl;
    thread T1(norace, 1);
    thread T2(norace, 2);
    
    T1.join();
    T2.join();

    cout << "Race" << endl;
    SharedData = {0,0};
    thread T3(race, 1);
    thread T4(race, 2);

    T3.join();
    T4.join();


} 