#pragma once

#include <map>
#include <string>

namespace threadserve {

struct HttpRequest {
    std::string method;
    std::string target;
    std::string path;
    std::string query;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

}  // namespace threadserve