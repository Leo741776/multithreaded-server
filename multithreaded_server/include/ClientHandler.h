#pragma once

#include "HTTPResponse.h"
#include "Router.h"

#include <string>
#include <winsock2.h>

class ClientHandler {
private:
    SOCKET m_clientSocket;
    Router m_router;

    std::string receiveRequest();
    void sendResponse(HTTPResponse response);
    void close();

public:
    ClientHandler(SOCKET socket);
    void handle();
};