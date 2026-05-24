#include "StaticFileHandler.h"

#include <fstream>
#include <sstream>

StaticFileHandler::StaticFileHandler(std::string root)
    : m_publicRoot(root) {
}

std::string StaticFileHandler::getContentType(std::string path) {
    size_t dotPos = path.find_last_of('.');

    if (dotPos == std::string::npos) {
        return "application/octet-stream";
    }

    std::string ext = path.substr(dotPos);

    if (ext == ".html") {
        return "text/html";
    }

    if (ext == ".css") {
        return "text/css";
    }

    if (ext == ".js") {
        return "application/javascript";
    }

    if (ext == ".png") {
        return "image/png";
    }

    if (ext == ".jpg" || ext == ".jpeg") {
        return "image/jpeg";
    }

    if (ext == ".txt") {
        return "text/plain";
    }

    return "application/octet-stream";
}

bool StaticFileHandler::isSafePath(std::string path) {
    return path.find("..") == std::string::npos;
}

HTTPResponse StaticFileHandler::serve(std::string path) {
    if (!isSafePath(path)) {
        return HTTPResponse(400, "Bad Request");
    }

    if (path == "/") {
        path = "/index.html";
    }

    std::string fullPath = m_publicRoot + path;

    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        return HTTPResponse(404, "<h1>404 Not Found</h1>");
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    HTTPResponse response(200, buffer.str());
    response.setHeader("Content-Type", getContentType(path));

    return response;
}