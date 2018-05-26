/*
 * =====================================================================================
 *
 *       Filename:  thread_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/16 14:54:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "thread.h"

class MyThread : public Thread{
private:
    int count;
public:
    MyThread() : count(0) {};
    int run() {
        while(count < 100) {
            printf("%d ", count ++);
        }
        return 1;
    }
};

int main(int argc, char* argv[]) {
    MyThread th;
    th.start();
    th.join();
    return 0;
}


