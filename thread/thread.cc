#include "thread.h"
#include <unistd.h>
#include <cassert>

const char* Thread::getName() const {
    return name_;
}

int Thread::getRet() const {
    return ret_;
}

pthread_t Thread::getTid() const {
    return tid_;
}

void Thread::setName(const char* name, const int size) {
    assert(size < name_size);
    snprintf(name_, size, "%s",name);
    return;
}

int Thread::start() {
    ret_ = pthread_create(&tid_, NULL, runFn_, this);
    return ret_;
}

int Thread::join() {
    while (0 == tid_) 
        usleep(sleep_time);
    ret_ = pthread_join(tid_, NULL);
    return ret_;
}

int Thread::detach() {
    while(0 == tid_)
        usleep(sleep_time);
    ret_ = pthread_detach(tid_);
    return ret_;
}

void* Thread::runFn_(void* para) {
    Thread* td = static_cast<Thread*>(para);
    pthread_t tid = td->getTid();
    while(0 == tid) 
        usleep(sleep_time);
    td->run();
    return 0;
}
