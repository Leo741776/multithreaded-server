#include "HTTPRequest.h"

#include <sstream>
#include <string>

HTTPRequest parse(std::string rawRequest) {
    HTTPRequest request;

    size_t bodyStart = rawRequest.find("\r\n\r\n");

    std::string headerPart;
    if (bodyStart != std::string::npos) {
        headerPart = rawRequest.substr(0, bodyStart);
        request.m_body = rawRequest.substr(bodyStart + 4);
    }
    else {
        headerPart = rawRequest;
        request.m_body = "";
    }

    std::istringstream stream(headerPart);

    std::string requestLine;
    std::getline(stream, requestLine);

    if (!requestLine.empty() && requestLine.back() == '\r') {
        requestLine.pop_back();
    }

    std::istringstream requestLineStream(requestLine);
    requestLineStream >> request.m_method >> request.m_path >> request.m_version;

    std::string headerLine;
    while (std::getline(stream, headerLine)) {
        if (!headerLine.empty() && headerLine.back() == '\r') {
            headerLine.pop_back();
        }

        size_t colonPos = headerLine.find(':');
        if (colonPos != std::string::npos) {
            std::string key = headerLine.substr(0, colonPos);
            std::string value = headerLine.substr(colonPos + 1);

            if (!value.empty() && value.front() == ' ') {
                value.erase(0, 1);
            }

            request.m_headers[key] = value;
        }
    }

    return request;
}