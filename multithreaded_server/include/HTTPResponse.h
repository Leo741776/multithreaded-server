#pragma once

#include <map>
#include <string>

class HTTPResponse {
private:
    int m_statusCode;
    std::string m_statusText;
    std::map<std::string, std::string> m_headers;
    std::string m_body;

public:
    HTTPResponse(int code, std::string body);
    void setHeader(std::string key, std::string value);
    std::string toString();
};