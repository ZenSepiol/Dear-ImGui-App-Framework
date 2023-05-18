#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "thread_save_queue.hpp"

class ThreadPool
{
  public:
    ThreadPool(const int n_threads) : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false)
    {
        for (size_t i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i] = std::thread(ThreadWorker(this, i));
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
            std::lock_guard<std::mutex> lock(m_conditional_mutex);
            m_shutdown = true;
            m_conditional_lock.notify_all();
        }

        for (size_t i = 0; i < m_threads.size(); ++i)
        {
            if (m_threads[i].joinable())
            {
                m_threads[i].join();
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

        // Enqueue generic wrapper function
        m_queue.enqueue(wrapper_func);

        // Wake up one thread if its waiting
        {
            std::lock_guard<std::mutex> lock(m_conditional_mutex);
            m_conditional_lock.notify_one();
        }
        // Return future from promise
        return task_ptr->get_future();
    }

  private:
    class ThreadWorker
    {
      public:
        ThreadWorker(ThreadPool* pool, const int id) : m_id(id), m_pool(pool)
        {
        }
        void operator()()
        {
            std::function<void()> func;
            bool dequeued;
            while (!m_pool->m_shutdown)
            {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    if (m_pool->m_queue.empty())
                    {
                        m_pool->m_conditional_lock.wait(lock, [this] {
                            return this->m_pool->m_shutdown || this->m_pool->m_queue.empty() == false;
                        });
                    }
                    dequeued = m_pool->m_queue.dequeue(func);
                }
                if (dequeued)
                {
                    func();
                }
            }

            // If the task queue is not empty, continue obtain task from task queue,
            // the multithread continues execution until the queue is empty
            while (!m_pool->m_queue.empty())
            {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    dequeued = m_pool->m_queue.dequeue(func);
                    if (dequeued)
                    {
                        func();
                    }
                }
            }
        }

      private:
        int m_id;
        ThreadPool* m_pool;
    };

    std::vector<std::thread> m_threads;
    bool m_shutdown;
    SafeQueue<std::function<void()>> m_queue;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;
};