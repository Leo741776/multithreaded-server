# Multithreaded HTTP Server

This is an HTTP server written in C++ using Winsock and a custom thread pool.

## How to Run

### Option 1 - Visual Studio

Open the project solution in Visual Studio and run it.
You can test it in a browser by visiting `http://localhost:8080/`

### Option 2 - CMake

From the project root, open Developer PowerShell and run:  
`cmake -S . -B build`  
`cmake --build build`  
Then start the server:  
`.\build\Debug\multithreaded_server.exe`

### Stress Test

In a second Developer PowerShell window, from the project root, run:  
`powershell -ExecutionPolicy Bypass -File .\test\stress-test.ps1`

A successful stress test should show each wave returning SUCCESS responses.

## Overview

- Creates a TCP listening socket with Winsock
- Accepts multiple client connections
- Handles each client request through a worker thread
- Parses basic HTTP GET requests
- Serves a static `index.html` file from `public/`
- Returns simple `404 Not Found` and `405 Method Not Allowed` responses
- Includes a PowerShell stress test for concurrent requests along with results from a test I performed in `test/`

## Design

This project is organized around a simple multithreaded HTTP server architecture.

### High-Level Flow

1. The server starts Winsock and creates a listening socket.
2. The socket is bound to port `8080`.
3. The server enters an accept loop and waits for clients.
4. When a client connects, `accept()` returns a new client socket.
5. The client socket is placed into a thread pool as a task.
6. A worker thread handles the client by:
   - receiving the raw HTTP request
   - parsing the request into an `HTTPRequest` object
   - routing the request
   - building an `HTTPResponse`
   - sending the response back to the client
   - closing the client socket

### Main Components

- **Server.cpp** - Initializes Winsock, creates the listening socket, binds, listens, accepts clients, and submits client work to the thread pool.
- **ThreadPool.cpp** - Maintains a fixed number of worker threads and a queue of client-handling tasks.
- **ClientHandler.cpp** - Handles one connected client socket by receiving the request, calling the router, sending the response, and closing the connection.
- **HTTPRequest.cpp** - Represents a parsed HTTP request, including method, path, version, headers, and body.
- **HTTPResponse.cpp** - Builds a valid HTTP response string with status line, headers, and body.
- **Router.cpp** - Decides how to respond based on the request method and path.
- **StaticFileHandler.cpp** - Reads files from the `public/` folder and returns them as HTTP responses.

## Concurrency Model

The server uses a fixed-size thread pool instead of creating a new thread for every client.

When a client connects, the main server thread accepts the connection and adds a task to the thread pool. One of the available worker threads then handles that client connection.

This allows multiple clients to be handled concurrently while avoiding the overhead of creating and destroying a thread for every request.
