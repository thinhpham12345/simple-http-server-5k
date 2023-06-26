#include "./thread-pool.hpp"

namespace utils
{

    ThreadsPool::ThreadsPool(size_t number_of_threads)
    {
        for (size_t thread_idx = 0; thread_idx < number_of_threads; thread_idx++)
        {
            _threads.emplace_back([this]
                                  {
                while (true) {
                    std::function<Handler_t> handler;
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        _condition.wait(lock, [this] { return _stop || !_handlers.empty(); });

                        if (_stop || _handlers.empty()) {
                            break;
                        }

                        handler = std::move(_handlers.front());
                        _handlers.pop();
                    }
                    handler();
                } });
        }
    }

    ThreadsPool::~ThreadsPool()
    {
        do
        {
        } while (!_handlers.empty());

        _stop = true;
        _condition.notify_all();

        for (auto &thread : _threads)
        {
            thread.join();
        }
    }

    void ThreadsPool::Enqueue(std::function<Handler_t> handler)
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _handlers.emplace(handler);
        }
        _condition.notify_one();
    }
}
