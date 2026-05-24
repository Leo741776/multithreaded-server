#pragma once

#include "HTTPResponse.h"

#include <string>

class StaticFileHandler {
private:
    std::string m_publicRoot;

    std::string getContentType(std::string path);
    bool isSafePath(std::string path);

public:
    StaticFileHandler(std::string root);
    HTTPResponse serve(std::string path);
};