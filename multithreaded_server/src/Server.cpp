#define WIN32_LEAN_AND_MEAN

#include "Server.h"
#include "ClientHandler.h"

#include <stdexcept>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

Server::Server(int port, int threadCount)
    : m_port(port),
    m_listenSocket(INVALID_SOCKET),
    m_threadPool(threadCount),
    m_running(false) {
}

void Server::acceptLoop() {
    while (m_running) {
        SOCKET clientSocket = accept(m_listenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            if (m_running) {
                throw std::runtime_error("accept() failed with error: " + std::to_string(WSAGetLastError()));
            }

            break;
        }

        m_threadPool.enqueue([clientSocket]() {
            ClientHandler handler(clientSocket);
            handler.handle();
            });
    }
}

void Server::cleanup() {
    m_running = false;

    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    WSACleanup();
}

void Server::start() {
    std::string portString = std::to_string(m_port);

    WSADATA wsaData;
    int iResult;

    addrinfo* result = nullptr;
    addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup() failed with error: " + std::to_string(iResult));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(nullptr, portString.c_str(), &hints, &result);
    if (iResult != 0) {
        WSACleanup();
        throw std::runtime_error("getaddrinfo() failed with error: " + std::to_string(iResult));
    }

    m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_listenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket() failed with error: " + std::to_string(WSAGetLastError()));
    }

    iResult = bind(m_listenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(m_listenSocket);
        WSACleanup();
        throw std::runtime_error("bind() failed with error: " + std::to_string(WSAGetLastError()));
    }

    freeaddrinfo(result);

    iResult = listen(m_listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        closesocket(m_listenSocket);
        WSACleanup();
        throw std::runtime_error("listen() failed with error: " + std::to_string(WSAGetLastError()));
    }

    m_running = true;
    acceptLoop();
}

void Server::stop() {
    cleanup();
}