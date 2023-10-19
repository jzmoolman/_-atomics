#include <iostream>
#include <vector>
#include <thread>


using namespace std;

// #define DEBUG_ENABLED

/*
 * Global Variables
*/

vector<int> SharedData; 
bool DataReady;
atomic<int> Atomic_DataReady = atomic<int>(0);

mutex mtx;
condition_variable cvar;

void SetDataReady() {
    SharedData = {1,0,3};

    lock_guard<mutex> lck(mtx);
    DataReady = true;

    cvar.notify_one();
}

void Atomic_SetDataReady() {
    SharedData = {1,0,3};
    Atomic_DataReady = 1;
}

void WaitForData() {
    unique_lock<mutex> lck(mtx);
    cvar.wait(lck, []{return DataReady;});
    SharedData[1] = 2;
}

void Atomic_WaitForData() {
    while(Atomic_DataReady.load() != 1)
       this_thread::sleep_for(chrono::milliseconds(5));
    SharedData[1] = 2;
}


int main(int argc, char** argv) {
    cout << "Atomic test program" << endl;
    thread T1(WaitForData);
    thread T2(SetDataReady);

    T1.join();
    T2.join();
    for ( int i = 0; i < 3; i++){
        cout << SharedData[i] << endl;
    }
    
    thread T3(Atomic_WaitForData);
    thread T4(Atomic_SetDataReady);

    T3.join();
    T4.join();
    for ( int i = 0; i < 3; i++){
        cout << SharedData[i] << endl;
    }

}