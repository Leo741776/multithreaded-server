#pragma once

#include <map>
#include <string>

struct HTTPRequest {
    std::string m_method;
    std::string m_path;
    std::string m_version;
    std::map<std::string, std::string> m_headers;
    std::string m_body;
};

HTTPRequest parse(std::string rawRequest);