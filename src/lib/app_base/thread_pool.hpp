#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool
{
  public:
    ThreadPool(const int size) :  busy_threads(size), threads(std::vector<std::thread>(size)), shutdown_requested(false)
    {
        for (size_t i = 0; i < size; ++i)
        {
            threads[i] = std::thread(ThreadWorker(this));
        }
    }

    ~ThreadPool()
    {
        shutdown();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)      = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    // Waits until threads finish their current task and shutdowns the pool
    void shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown_requested = true;
            condition_variable.notify_all();
        }

        for (size_t i = 0; i < threads.size(); ++i)
        {
            if (threads[i].joinable())
            {
                threads[i].join();
            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };

        // Wake up one thread if its waiting
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(wrapper_func);
            condition_variable.notify_one();
        }
        // Return future from promise
        return task_ptr->get_future();
    }

  private:
    class ThreadWorker
    {
      public:
        ThreadWorker(ThreadPool* pool) : thread_pool(pool)
        {
        }

        void operator()()
        {
            std::unique_lock<std::mutex> lock(thread_pool->mutex);
            while (!thread_pool->shutdown_requested || (thread_pool->shutdown_requested && !thread_pool->queue.empty()))
            {
                thread_pool->busy_threads--;
                thread_pool->condition_variable.wait(lock, [this] {
                    return this->thread_pool->shutdown_requested || !this->thread_pool->queue.empty();
                });
                thread_pool->busy_threads++;

                if (!this->thread_pool->queue.empty())
                {
                    auto func = thread_pool->queue.front();
                    thread_pool->queue.pop();

                    lock.unlock();
                    func();
                    lock.lock();
                }
            }
        }

      private:
        ThreadPool* thread_pool;
    };

  public:
    int busy_threads;

  private:
    mutable std::mutex mutex;
    std::condition_variable condition_variable;

    std::vector<std::thread> threads;
    bool shutdown_requested;

    std::queue<std::function<void()>> queue;
};