#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define Handler_t void()

namespace utils
{
    class ThreadsPool
    {
    public:
        ThreadsPool() = delete;
        explicit ThreadsPool(size_t number_of_threads);
        ~ThreadsPool();

    public:
        void Enqueue(std::function<Handler_t> handler);

    private:
        std::vector<std::thread> _threads;
        std::queue<std::function<Handler_t>> _handlers;
        std::mutex _mutex;
        std::condition_variable _condition;

    private:
        std::atomic<bool> _stop = false;
    };
}

#endif
