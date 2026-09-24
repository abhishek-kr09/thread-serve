#pragma once

#include <map>
#include <string>

namespace threadserve {

enum class HttpStatus : int {
    ok = 200,
    created = 201,
    no_content = 204,
    bad_request = 400,
    not_found = 404,
    internal_server_error = 500,
};

class HttpResponse {
public:
    HttpResponse(HttpStatus status, std::string body);

    void set_header(std::string name, std::string value);

    [[nodiscard]] std::string serialize() const;

private:
    HttpStatus status_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

}  // namespace threadserve