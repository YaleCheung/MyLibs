#ifndef TASKQUEUE_HHH
#define TASKQUEUE_HHH
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>

template<class T>
class TaskQueue
{
 public:
    TaskQueue(int capacity = 0)
        : _capacity(capacity), _size(0), _is_close(false)
    {
    }

    template<class U>
    void Push(U &&elem)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_capacity > 0) {
            _push_cond.wait(lock, [this]{ return _queue.size() < _capacity; });
        }
        _queue.push_back(std::forward<U>(elem));
        _size = _queue.size();
        lock.unlock();
        _pop_cond.notify_one();
    }

    template<class U>
    void PushFront(U &&elem)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push_front(std::forward<U>(elem));
        _size = _queue.size();
        lock.unlock();
        _pop_cond.notify_one();
    }

    bool Pop(T &elem, int64_t timeout_us = -1)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (timeout_us < 0) {
            _pop_cond.wait(lock, [this]{ return !_queue.empty() || _is_close; });
        } else {
            if (!_pop_cond.wait_for(lock, std::chrono::microseconds(timeout_us),
                                 [this]{ return !_queue.empty() || _is_close; })) {
                return false;
            }
        }
        if (_queue.empty()) {
            return false;
        }
        elem = std::move(_queue.front());
        _queue.pop_front();
        _size = _queue.size();
        lock.unlock();
        if (_capacity > 0) {
            _push_cond.notify_one();
        }
        return true;
    }

    void Close()
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _is_close = true;
        }
        _push_cond.notify_all();
        _pop_cond.notify_all();
    }

    bool IsClose() const
    {
        return _is_close;
    }

    int Size() const
    {
        return _size;
    }

 private:
    std::deque<T> _queue;
    int _capacity;
    std::atomic<int> _size;
    std::mutex _mutex;
    std::condition_variable _push_cond;
    std::condition_variable _pop_cond;
    std::atomic<bool> _is_close;
};
#endif  //TASKQUEUE_HHH
