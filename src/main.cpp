#include "threadserve/server.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        threadserve::TcpServer server(8080);
        server.start();
        server.run();
    } catch (const std::exception& error) {
        std::cerr << "ThreadServe failed: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
