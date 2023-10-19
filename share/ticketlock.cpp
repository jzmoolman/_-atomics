
#include "../include/ticketlock.h"
#include "../include/zutils.h"

void TicketLock::lock() {
    int _number = fai(next_num, 1, std::memory_order_seq_cst); // think can be aquire and release
    while(now_serving.load(std::memory_order_seq_cst) != _number) {}
} 

void TicketLock::unlock() {
    fai(now_serving, 1, std::memory_order_seq_cst);
}