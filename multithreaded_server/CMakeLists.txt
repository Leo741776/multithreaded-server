cmake_minimum_required(VERSION 3.20)

project(multithreaded_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(multithreaded_server
    src/main.cpp
    src/Server.cpp
    src/ClientHandler.cpp
    src/HTTPRequest.cpp
    src/HTTPResponse.cpp
    src/Router.cpp
    src/StaticFileHandler.cpp
    src/ThreadPool.cpp
)

target_include_directories(multithreaded_server PRIVATE
    include
)

if(WIN32)
    target_link_libraries(multithreaded_server PRIVATE ws2_32)
endif()

add_custom_command(TARGET multithreaded_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/public
        $<TARGET_FILE_DIR:multithreaded_server>/public
)