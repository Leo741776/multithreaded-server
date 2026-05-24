#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stopping;

public:
    ThreadPool(int threadCount);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void stop();
};