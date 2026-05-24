#pragma once

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "StaticFileHandler.h"

class Router {
private:
    StaticFileHandler m_staticFileHandler;

public:
    Router();
    HTTPResponse route(HTTPRequest request);
};