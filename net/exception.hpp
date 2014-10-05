#pragma once

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

namespace net {
    class exception : public std::runtime_error {
    public:
        exception(const std::string& what = "net::exception") : std::runtime_error{what} {}
    };

    class errno_exception : public exception {
    public:
        errno_exception() : exception{"errno " + std::to_string(errno) + " : " + std::strerror(errno)} {}
    };

    class connection_closed : public exception {
    public:
        connection_closed() : exception{"connectiong has been closed by the remote side"} {}
    };
}
