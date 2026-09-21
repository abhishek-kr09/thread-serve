#pragma once

#include "threadserve/tcp_socket.hpp"

#include <atomic>
#include <cstdint>

namespace threadserve {

class TcpServer {
public:
    explicit TcpServer(std::uint16_t port = 8080);

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    void start();
    void run();
    void stop() noexcept;

    [[nodiscard]] bool running() const noexcept;
    [[nodiscard]] std::uint16_t port() const noexcept;

private:
    std::uint16_t port_;
    TcpSocket listener_;
    std::atomic<bool> running_{false};
};

}  // namespace threadserve