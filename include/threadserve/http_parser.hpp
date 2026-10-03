#pragma once

#include "threadserve/http_request.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace threadserve {

struct HttpParserLimits {
    std::size_t max_header_bytes{16 * 1024};
    std::size_t max_body_bytes{1024 * 1024};
    std::size_t max_header_count{100};
};

class HttpParseError : public std::runtime_error {
public:
    HttpParseError(int status_code, std::string message);

    [[nodiscard]] int status_code() const noexcept;

private:
    int status_code_;
};

class TcpSocket;

[[nodiscard]] HttpRequest read_http_request(
    TcpSocket& socket, const HttpParserLimits& limits = {});

}  // namespace threadserve