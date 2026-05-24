#pragma once

#include "ThreadPool.h"

#include <atomic>
#include <winsock2.h>

class Server {
private:
    int m_port;
    SOCKET m_listenSocket;
    ThreadPool m_threadPool;
    std::atomic<bool> m_running;

    void acceptLoop();
    void cleanup();

public:
    Server(int port, int threadCount);
    void start();
    void stop();
};