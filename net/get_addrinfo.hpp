#pragma once

#include "exception.hpp"
#include <netdb.h>
#include <string>
#include <memory>

namespace net {
    struct addrinfo_deleter { void operator()(addrinfo* ptr) const { freeaddrinfo(ptr); } };
    using addrinfo_ptr_t = std::unique_ptr<addrinfo, addrinfo_deleter>;

    addrinfo_ptr_t get_addrinfo(int sock_type, const std::string& address, const std::string& port);
}
