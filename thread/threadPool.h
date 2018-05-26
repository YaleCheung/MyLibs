/*
 * =====================================================================================
 *
 *       Filename:  threadPool.h
 *
 *    Description:  a thread pool in c++11;
 *
 *        Version:  1.0
 *        Created:  08/11/16 09:53:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangyi
 *   Organization:  CETC
 *
 * =====================================================================================
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <functional>
#include <future>
#include <exception>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <queue>
#include <utility>

enum {default_thread_num = 2};
class ThreadPool {
    // custom
    std::deque<std::thread> workers_;
    // producer
    std::queue<std::function<void()>> tasks_;
    // queue_lock is used to protect the thread
    std::mutex queue_lock_;
    // condition_variable is used to call threads to work
    std::condition_variable condition_variable_;
    // if the variable stop is set to true, then all the thread will join()
    bool stop_; 
public:
    ThreadPool(size_t thread_num);
    template<class F, class... Args>
        auto enQueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
};

 /** 
  *@ Description: Construct a thread pool
  *@ Author: zy
  *@ Paras: thread_num means the number of threads in the pool;
  **/

ThreadPool::ThreadPool(size_t thread_num = default_thread_num) : 
    stop_ (false) {
    for(size_t i = 0; i < thread_num; ++ i) {
        workers_.emplace_back(
            [this] {
            for(;;) {
                std::function<void()> task;
                // a small region protected by lock;
                {
                    std::unique_lock<std::mutex> lock(this -> queue_lock_);
                    this -> condition_variable_.wait(lock, [this] {
                        return !tasks_.empty() || this->stop_;
                        });
                    // get the task;
                    if (this->stop_ && this->tasks_.empty())
                        return;
                    task = std::move(tasks_.front());
                    this -> tasks_.pop();
                }
                task();
            }
            });
    }
}

/** 
*@ Description: add a task to the pool
*@ Author: zy
*@ Paras: args... ;
**/
template<class F, class... Args>
auto ThreadPool::enQueue(F&& f, Args&&... args)-> std::future<typename std::result_of<F(Args...)>::type> {
    using RetType = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<RetType> ret = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_lock_);
        if (stop_)
            throw std::runtime_error("enuque stoped");
        tasks_.emplace([task]() {(*task)();});
    }
    condition_variable_.notify_one();
    return ret;
}

/** 
*@ Description: destruct a threadpool
*@ Author: zy
*@ Paras: none;
**/

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(this -> queue_lock_);
        stop_ = true;
    }
    condition_variable_.notify_all();
    for(auto& worker : workers_)
        worker.join();
}
#endif
