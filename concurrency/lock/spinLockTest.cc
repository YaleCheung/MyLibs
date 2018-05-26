#include "spinLock.h"
#include <thread>
#include <iostream>
#include <chrono>



using namespace std::chrono_literals;
using std::cout;
using std::thread;
int main(int argc, char* argv[]) {
    bool data_race = false;
    SpinLock lock;

    thread a([&data_race, &lock] {
        std::cout << "thread a try to lock" << '\n';
        lock.Lock();
        std::cout << "thread a get lock" << '\n';
        data_race = true;
        std::this_thread::sleep_for(10s);
        std::cout << "thread a release lock" << '\n';
        lock.UnLock();
         });
    thread b([&data_race, &lock] {
        std::cout << "thread b try to lock" << '\n';
        lock.Lock();
        std::cout << "thread b get lock" << '\n';
        data_race = false;
        std::this_thread::sleep_for(20s);
        std::cout << "thread b release lock" << '\n';
        lock.UnLock();
         });
    a.join();
    b.join();
    return 0;
}
