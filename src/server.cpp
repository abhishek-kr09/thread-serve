#include "threadserve/server.hpp"

#include "threadserve/http_response.hpp"

#include <iostream>
#include <stdexcept>

namespace threadserve {

TcpServer::TcpServer(std::uint16_t port) : port_(port) {}

void TcpServer::start() {
    if (running()) {
        throw std::logic_error("server is already running");
    }

    listener_ = TcpSocket::create();
    listener_.set_reuse_address();
    listener_.bind(port_);
    listener_.listen(16);
    running_.store(true);

    std::cout << "ThreadServe listening on 0.0.0.0:" << port_ << '\n';
}

void TcpServer::run() {
    if (!running()) {
        throw std::logic_error("server must be started before run");
    }

    while (running()) {
        try {
            TcpSocket client = listener_.accept();
            std::cout << "Accepted TCP connection\n";

            HttpResponse response(HttpStatus::ok, "ThreadServe is running\n");
            response.set_header("Content-Type", "text/plain; charset=utf-8");
            client.send_all(response.serialize());
        } catch (const std::runtime_error& error) {
            if (running()) {
                throw;
            }
            std::cerr << "TCP accept loop stopped: " << error.what() << '\n';
        }
    }
}

void TcpServer::stop() noexcept {
    if (!running_.exchange(false)) {
        return;
    }
    listener_.close();
    std::cout << "ThreadServe stopped\n";
}

bool TcpServer::running() const noexcept {
    return running_.load();
}

std::uint16_t TcpServer::port() const noexcept {
    return port_;
}

}  // namespace threadserve