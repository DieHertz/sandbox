#include "get_addrinfo.hpp"

namespace net {
    addrinfo_ptr_t get_addrinfo(const int sock_type, const std::string& address, const std::string& port) {
        const auto use_own_address = address.empty();

        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = sock_type;
        if (use_own_address) hints.ai_flags = AI_PASSIVE;

        addrinfo* info{};
        const auto status = getaddrinfo(use_own_address ? nullptr : address.data(), port.data(), &hints, &info);
        if (status != 0) throw exception{gai_strerror(status)};

        return addrinfo_ptr_t{info};
    }
}
