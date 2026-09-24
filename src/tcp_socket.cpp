#include "threadserve/tcp_socket.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace threadserve {
namespace {

#ifdef _WIN32
class WinsockRuntime {
public:
    WinsockRuntime() {
        WSADATA data{};
        const int result = WSAStartup(MAKEWORD(2, 2), &data);
        if (result != 0) {
            throw std::runtime_error("WSAStartup failed with error " + std::to_string(result));
        }
    }

    ~WinsockRuntime() {
        WSACleanup();
    }
};

void ensure_winsock_initialized() {
    static WinsockRuntime runtime;
    (void)runtime;
}
#else
void ensure_winsock_initialized() {}
#endif

[[nodiscard]] std::string socket_error_message(const char* operation) {
#ifdef _WIN32
    return std::string(operation) + " failed with error " +
           std::to_string(WSAGetLastError());
#else
    return std::string(operation) + " failed: " + std::strerror(errno);
#endif
}

void close_native_socket(NativeSocket handle) noexcept {
    if (handle == invalid_socket) {
        return;
    }
#ifdef _WIN32
    closesocket(static_cast<SOCKET>(handle));
#else
    ::close(static_cast<int>(handle));
#endif
}

}  // namespace

TcpSocket::TcpSocket(NativeSocket handle) noexcept : handle_(handle) {}

TcpSocket::~TcpSocket() {
    close();
}

TcpSocket::TcpSocket(TcpSocket&& other) noexcept : handle_(other.handle_) {
    other.handle_ = invalid_socket;
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = invalid_socket;
    }
    return *this;
}

TcpSocket TcpSocket::create() {
    ensure_winsock_initialized();
#ifdef _WIN32
    const auto handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (handle == INVALID_SOCKET) {
        throw std::runtime_error(socket_error_message("socket"));
    }
    return TcpSocket(static_cast<NativeSocket>(handle));
#else
    const int handle = ::socket(AF_INET, SOCK_STREAM, 0);
    if (handle < 0) {
        throw std::runtime_error(socket_error_message("socket"));
    }
    return TcpSocket(static_cast<NativeSocket>(handle));
#endif
}

void TcpSocket::set_reuse_address() {
    int enabled = 1;
#ifdef _WIN32
    const int result = ::setsockopt(
        static_cast<SOCKET>(handle_), SOL_SOCKET, SO_REUSEADDR,
        reinterpret_cast<const char*>(&enabled), sizeof(enabled));
#else
    const int result = ::setsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));
#endif
    if (result != 0) {
        throw std::runtime_error(socket_error_message("setsockopt"));
    }
}

void TcpSocket::bind(std::uint16_t port) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

#ifdef _WIN32
    const int result = ::bind(
        static_cast<SOCKET>(handle_), reinterpret_cast<const sockaddr*>(&address), sizeof(address));
#else
    const int result = ::bind(handle_, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
#endif
    if (result != 0) {
        throw std::runtime_error(socket_error_message("bind"));
    }
}

void TcpSocket::listen(int backlog) {
#ifdef _WIN32
    const int result = ::listen(static_cast<SOCKET>(handle_), backlog);
#else
    const int result = ::listen(handle_, backlog);
#endif
    if (result != 0) {
        throw std::runtime_error(socket_error_message("listen"));
    }
}

void TcpSocket::send_all(const std::string& data) {
    std::size_t bytes_sent = 0;
    while (bytes_sent < data.size()) {
#ifdef _WIN32
        const int result = ::send(static_cast<SOCKET>(handle_), data.data() + bytes_sent,
                                  static_cast<int>(data.size() - bytes_sent), 0);
#else
        const auto result = ::send(handle_, data.data() + bytes_sent, data.size() - bytes_sent, 0);
#endif
        if (result <= 0) {
            throw std::runtime_error(socket_error_message("send"));
        }
        bytes_sent += static_cast<std::size_t>(result);
    }
}

TcpSocket TcpSocket::accept() {
#ifdef _WIN32
    const SOCKET client = ::accept(static_cast<SOCKET>(handle_), nullptr, nullptr);
    if (client == INVALID_SOCKET) {
        throw std::runtime_error(socket_error_message("accept"));
    }
    return TcpSocket(static_cast<NativeSocket>(client));
#else
    const int client = ::accept(handle_, nullptr, nullptr);
    if (client < 0) {
        throw std::runtime_error(socket_error_message("accept"));
    }
    return TcpSocket(static_cast<NativeSocket>(client));
#endif
}

void TcpSocket::close() noexcept {
    close_native_socket(handle_);
    handle_ = invalid_socket;
}

bool TcpSocket::valid() const noexcept {
    return handle_ != invalid_socket;
}

NativeSocket TcpSocket::native_handle() const noexcept {
    return handle_;
}

}  // namespace threadserve