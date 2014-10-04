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

    class invalid_address : public exception {
    public:
        invalid_address(const std::string& address) : exception{"the address is invalid: `" + address + '`'} {}
    };

    class errno_exception : public exception {
    public:
        errno_exception() : exception{"errno " + std::to_string(errno) + " : " + std::strerror(errno)} {}
    };
}
