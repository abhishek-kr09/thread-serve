#include "threadserve/http_parser.hpp"

#include "threadserve/tcp_socket.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>
#include <utility>

namespace threadserve {
namespace {

[[nodiscard]] std::string trim(std::string value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char character) {
        return std::isspace(character) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char character) {
        return std::isspace(character) != 0;
    }).base();
    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

[[nodiscard]] std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

[[nodiscard]] std::size_t parse_content_length(const std::string& value) {
    const std::string trimmed = trim(value);
    if (trimmed.empty() ||
        !std::all_of(trimmed.begin(), trimmed.end(), [](unsigned char character) {
            return std::isdigit(character) != 0;
        })) {
        throw HttpParseError(400, "invalid Content-Length");
    }

    try {
        return std::stoull(trimmed);
    } catch (const std::exception&) {
        throw HttpParseError(400, "invalid Content-Length");
    }
}

void parse_request_line(const std::string& line, HttpRequest& request) {
    std::istringstream input(line);
    if (!(input >> request.method >> request.target >> request.version)) {
        throw HttpParseError(400, "malformed request line");
    }

    std::string extra;
    if (input >> extra) {
        throw HttpParseError(400, "malformed request line");
    }
    if (request.version != "HTTP/1.0" && request.version != "HTTP/1.1") {
        throw HttpParseError(505, "unsupported HTTP version");
    }

    const std::size_t query_start = request.target.find('?');
    if (query_start == std::string::npos) {
        request.path = request.target;
    } else {
        request.path = request.target.substr(0, query_start);
        request.query = request.target.substr(query_start + 1);
    }
    if (request.path.empty() || request.path.front() != '/') {
        throw HttpParseError(400, "invalid request target");
    }
}

void parse_headers(const std::string& header_text, HttpRequest& request,
                   const HttpParserLimits& limits) {
    std::istringstream input(header_text);
    std::string line;
    std::size_t header_count = 0;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            break;
        }
        const std::size_t separator = line.find(':');
        if (separator == std::string::npos || separator == 0) {
            throw HttpParseError(400, "malformed header");
        }
        if (++header_count > limits.max_header_count) {
            throw HttpParseError(400, "too many headers");
        }
        const std::string name = lowercase(trim(line.substr(0, separator)));
        const std::string value = trim(line.substr(separator + 1));
        if (name.empty()) {
            throw HttpParseError(400, "empty header name");
        }
        request.headers[name] = value;
    }
}

}  // namespace

HttpParseError::HttpParseError(int status_code, std::string message)
    : std::runtime_error(std::move(message)), status_code_(status_code) {}

int HttpParseError::status_code() const noexcept {
    return status_code_;
}

HttpRequest read_http_request(TcpSocket& socket, const HttpParserLimits& limits) {
    std::string raw;
    std::size_t header_end = std::string::npos;
    char buffer[4096];

    while (header_end == std::string::npos) {
        const std::size_t bytes_read = socket.receive_some(buffer, sizeof(buffer));
        if (bytes_read == 0) {
            throw HttpParseError(400, "connection closed before headers");
        }
        raw.append(buffer, bytes_read);
        header_end = raw.find("\r\n\r\n");
        if (header_end == std::string::npos && raw.size() > limits.max_header_bytes) {
            throw HttpParseError(400, "headers too large");
        }
    }

    const std::size_t header_bytes = header_end + 4;
    if (header_bytes > limits.max_header_bytes) {
        throw HttpParseError(400, "headers too large");
    }

    HttpRequest request;
    const std::size_t request_line_end = raw.find("\r\n");
    if (request_line_end == std::string::npos || request_line_end > header_end) {
        throw HttpParseError(400, "missing request line");
    }
    parse_request_line(raw.substr(0, request_line_end), request);
    parse_headers(raw.substr(request_line_end + 2, header_end - request_line_end - 2), request,
                  limits);

    std::size_t content_length = 0;
    const auto content_length_header = request.headers.find("content-length");
    if (content_length_header != request.headers.end()) {
        content_length = parse_content_length(content_length_header->second);
    }
    if (content_length > limits.max_body_bytes) {
        throw HttpParseError(413, "request body too large");
    }

    while (raw.size() - header_bytes < content_length) {
        const std::size_t bytes_read = socket.receive_some(buffer, sizeof(buffer));
        if (bytes_read == 0) {
            throw HttpParseError(400, "connection closed before body was complete");
        }
        raw.append(buffer, bytes_read);
        if (raw.size() - header_bytes > limits.max_body_bytes) {
            throw HttpParseError(413, "request body too large");
        }
    }

    request.body = raw.substr(header_bytes, content_length);
    return request;
}

}  // namespace threadserve