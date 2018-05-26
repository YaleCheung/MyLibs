/*
 * =====================================================================================
 *
 *       Filename:  thread.h
 *
 *    Description:  // a light wrap of linux p-thread, later win thread will be included
 *
 *        Version:  1.0
 *        Created:  07/05/16 10:14:51
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  zhangyi, 
 *   Organization:  CETC
 *
 * =====================================================================================
 */

#ifndef THREAD_HHHH
#define THREAD_HHHH

#include <strings.h>
#include <pthread.h>
#include <strings.h>
#include <stdio.h>

enum {sleep_time = 10000};
enum {name_size = 256};

class Thread {
private:
    // thread id
    pthread_t tid_;
    // thread name
    char name_[name_size];
    // return ret;
    int ret_;
    // thread subrutine
    static void* RunFn_(void* para);
public:
    Thread() :
        tid_(0), ret_(1) {
            bzero(name_, sizeof(name_));
        }
    virtual ~Thread(){};
    // get thread name;
    const char* getName() const;
    // get the return result;
    int getRet() const;
    // get the thread ID
    pthread_t getTid() const;
    // set thread name; 
    void setName(const char* name, const int size);
    // start the thread;
    int start();
    // run the thread;
    virtual int run() = 0;
    // wait until the thread is runover
    int join();
    // detach the thread, which couldnot be under-controll
    int detach();
};

#endif
