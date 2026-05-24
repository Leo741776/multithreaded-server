#include "Server.h"

#include <iostream>
#include <stdexcept>

#define DEFAULT_PORT 8080
#define THREAD_COUNT 4

int main() {
    try {
        Server server{ DEFAULT_PORT, THREAD_COUNT };

        std::cout << "Server running on http://localhost:" << DEFAULT_PORT << '\n';
        server.start();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Error: " << e.what() << '\n';
    }

    return 0;
}