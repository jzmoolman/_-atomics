#include <iostream>
#include <thread>

using namespace std;

// #define DEBUG_ENABLED

/*
 * Global Variables
*/

vector<int> SharedData;
atomic<bool> data_produced;
atomic<bool> data_consumed;

void dataProceder() {
    SharedData = {1, 0, 3};
    data_produced.store(true, memory_order_release);
}

void dataDelivery() {
    while( !data_produced.load(memory_order_acquire));
    data_consumed.store(true, memory_order_release);

}

void dataConsumer() {
    while (! data_consumed.load(memory_order_acquire));
    SharedData[1] = 2;
}

int main(int argc, char** argv) {
    cout << "Atomic test program" << endl;
    thread T1(dataProceder);
    thread T2(dataDelivery);
    thread T3(dataConsumer);

    T1.join();
    T2.join();
    T3.join();

    for ( int i = 0; i < 3; i++){
        cout << SharedData[i] << endl;
    }

} 