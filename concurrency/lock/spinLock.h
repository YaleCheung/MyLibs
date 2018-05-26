/**
 *@ Describe: a lock free implemented lock. this is a toy, cannot be used in real work. for the CPU consumption.
 *@ Author:   zy
 *@ Org:      None
 *@ Date:     Thu Feb  1 10:13:35 CST 2018
 **/

#pragma once

#include <atomic>
using atomic_flag = std::atomic_flag;

class SpinLock {
public:
    SpinLock() {}
    ~SpinLock() {}
    // lock with acquire 
    void Lock() {
        while(flag_.test_and_set(std::memory_order_acquire));
    }
    // unlock by release, atomic with acquire will see the operation
    void UnLock() {
        flag_.clear(std::memory_order_relaxed);
    }
private:
    atomic_flag flag_;    // a basic atomic structure
};
