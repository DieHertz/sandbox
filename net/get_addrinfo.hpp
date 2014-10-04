#pragma once

#include "exception.hpp"
#include <netdb.h>
#include <string>
#include <memory>

namespace net {
    struct addrinfo_deleter { void operator()(addrinfo* ptr) const { freeaddrinfo(ptr); } };
    using addrinfo_ptr_t = std::unique_ptr<addrinfo, addrinfo_deleter>;

    addrinfo_ptr_t get_addrinfo(const std::string& address, const std::string& port) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if (address.empty()) hints.ai_flags = AI_PASSIVE;

        addrinfo* info{};
        const auto status = getaddrinfo(address.data(), port.data(), &hints, &info);
        if (status != 0) throw exception{gai_strerror(status)};

        return addrinfo_ptr_t{info};
    }
}
