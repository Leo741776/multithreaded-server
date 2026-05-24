#include "HTTPResponse.h"

#include <string>

static std::string getStatusText(int code) {
    switch (code) {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 500:
        return "Internal Server Error";
    default:
        return "Unknown";
    }
}

HTTPResponse::HTTPResponse(int code, std::string body)
    : m_statusCode(code),
    m_statusText(getStatusText(code)),
    m_body(body) {
    setHeader("Content-Length", std::to_string(m_body.size()));
    setHeader("Content-Type", "text/html");
    setHeader("Connection", "close");
}

void HTTPResponse::setHeader(std::string key, std::string value) {
    m_headers[key] = value;
}

std::string HTTPResponse::toString() {
    std::string response;

    response += "HTTP/1.1 " + std::to_string(m_statusCode) + " " + m_statusText + "\r\n";

    for (const auto& header : m_headers) {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";
    response += m_body;

    return response;
}