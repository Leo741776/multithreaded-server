#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadCount)
    : m_stopping(false) {
    for (int i = 0; i < threadCount; i++) {
        m_workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m_mutex);

                    m_condition.wait(lock, [this]() {
                        return m_stopping || !m_tasks.empty();
                        });

                    if (m_stopping && m_tasks.empty()) {
                        return;
                    }

                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }

                task();
            }
            });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stopping) {
            return;
        }

        m_tasks.push(std::move(task));
    }

    m_condition.notify_one();
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stopping) {
            return;
        }

        m_stopping = true;
    }

    m_condition.notify_all();

    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}