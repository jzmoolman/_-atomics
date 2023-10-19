
#ifndef _ZUTILS_H_
#define _ZUTILS_H_

#include <atomic>

bool tas(std::atomic<bool> &a, std::memory_order order);

int fai(std::atomic<int> &a, int amount, std::memory_order order);

template <typename T>
bool cas(std::atomic<T> &a, T expected, T desired, std::memory_order order);

// template <typename T> Joe's verion
// T vcas(std::atomic<T> &a, T expected, T desired, std::memory_order order);

template <typename T>
bool vcas(std::atomic<T> &a, T &expected, T desired, std::memory_order order);

#endif