#include "threadserve/http_response.hpp"

#include <sstream>
#include <utility>

namespace threadserve {
namespace {

[[nodiscard]] const char* reason_phrase(HttpStatus status) {
    switch (status) {
        case HttpStatus::ok:
            return "OK";
        case HttpStatus::created:
            return "Created";
        case HttpStatus::no_content:
            return "No Content";
        case HttpStatus::bad_request:
            return "Bad Request";
        case HttpStatus::not_found:
            return "Not Found";
        case HttpStatus::internal_server_error:
            return "Internal Server Error";
    }
    return "Unknown Status";
}

}  // namespace

HttpResponse::HttpResponse(HttpStatus status, std::string body)
    : status_(status), body_(std::move(body)) {}

void HttpResponse::set_header(std::string name, std::string value) {
    headers_[std::move(name)] = std::move(value);
}

std::string HttpResponse::serialize() const {
    std::ostringstream output;
    output << "HTTP/1.1 " << static_cast<int>(status_) << ' ' << reason_phrase(status_) << "\r\n";

    auto headers = headers_;
    headers["Content-Length"] = std::to_string(body_.size());
    headers["Connection"] = "close";

    for (const auto& header : headers) {
        output << header.first << ": " << header.second << "\r\n";
    }

    output << "\r\n" << body_;
    return output.str();
}

}  // namespace threadserve