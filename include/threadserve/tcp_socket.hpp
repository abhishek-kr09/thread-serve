#pragma once

#include <cstdint>

namespace threadserve {

#ifdef _WIN32
using NativeSocket = std::uintptr_t;
#else
using NativeSocket = int;
#endif

constexpr NativeSocket invalid_socket = static_cast<NativeSocket>(-1);

class TcpSocket {
public:
    TcpSocket() noexcept = default;
    explicit TcpSocket(NativeSocket handle) noexcept;
    ~TcpSocket();

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    TcpSocket(TcpSocket&& other) noexcept;
    TcpSocket& operator=(TcpSocket&& other) noexcept;

    static TcpSocket create();

    void set_reuse_address();
    void bind(std::uint16_t port);
    void listen(int backlog);
    TcpSocket accept();
    void close() noexcept;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] NativeSocket native_handle() const noexcept;

private:
    NativeSocket handle_{invalid_socket};
};

}  // namespace threadserve