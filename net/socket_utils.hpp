#pragma once

#include "socket_type.hpp"
#include <cstdint>
#include <string>

namespace net {
    int create_socket_fd(socket_type type, const std::string& address, const std::uint16_t port);

    enum struct select_type {
        read,
        write,
    };
    bool is_ready(int socket_fd, select_type type, std::int32_t timeout_msec);
}
