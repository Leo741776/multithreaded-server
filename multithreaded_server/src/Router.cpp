#include "Router.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

Router::Router()
    : m_staticFileHandler("public") {
}

HTTPResponse Router::route(HTTPRequest request) {
    if (request.m_method != "GET") {
        HTTPResponse response(405, "Method Not Allowed");
        response.setHeader("Content-Type", "text/plain");

        return response;
    }

    if (request.m_path == "/" || request.m_path == "/index.html") {
        return m_staticFileHandler.serve("/");
    }

    if (request.m_path == "/about") {
        return HTTPResponse(200, "<h1>About Page</h1>");
    }

    return HTTPResponse(404, "<h1>404 Not Found</h1>");
}