#include "ClientHandler.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

constexpr int defaultBuflen = 512;

ClientHandler::ClientHandler(SOCKET socket)
    : m_clientSocket(socket) {
}

std::string ClientHandler::receiveRequest() {
    char buffer[defaultBuflen];

    int bytesReceived = recv(m_clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        return std::string(buffer, bytesReceived);
    }

    return "";
}

void ClientHandler::sendResponse(HTTPResponse response) {
    std::string responseText = response.toString();

    send(m_clientSocket, responseText.c_str(), static_cast<int>(responseText.size()), 0);
}

void ClientHandler::close() {
    if (m_clientSocket != INVALID_SOCKET) {
        shutdown(m_clientSocket, SD_SEND);
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }
}

void ClientHandler::handle() {
    std::string rawRequest = receiveRequest();

    if (!rawRequest.empty()) {
        HTTPRequest request = parse(rawRequest);
        HTTPResponse response = m_router.route(request);

        sendResponse(response);
    }

    close();
}