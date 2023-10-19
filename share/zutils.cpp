#include "../include/zutils.h"

bool tas(std::atomic<bool> &a, std::memory_order order){
    bool expected = false;
    bool desired = true;
    return a.compare_exchange_strong(expected, desired, order);
}

int fai(std::atomic<int> &a, int amount, std::memory_order order){
    return a.fetch_add(amount, order);
}

// Limitation of templates - should be in header file or explicit implementaiton

template <typename T>
bool cas(std::atomic<T> &a, T expected, T desired, std::memory_order order)
{
    T expected_ref = expected;
    return a.compare_exchange_strong(expected_ref, desired, order);
}

template<> 
bool cas<void*>(std::atomic<void*> &a, void* expected, void* desired, std::memory_order order){
    void* expected_ref = expected;
    return a.compare_exchange_strong(expected_ref, desired, order);
}


// template <> Joe's version
// void* vcas<void*>(std::atomic<void*> &a, void* expected, void* desired, std::memory_order order) {
//     void* expected_ref = expected;
//     a.compare_exchange_strong(expected_ref, desired, order);
//     return expected_ref;
// }

template <>
bool vcas<void*>(std::atomic<void*> &a, void* &expected, void* desired, std::memory_order order) {
    return a.compare_exchange_strong(expected, desired, order);
}
