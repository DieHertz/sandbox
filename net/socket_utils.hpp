#pragma once

#include "socket_type.hpp"
#include <cstdint>
#include <string>

namespace net {
    int create_socket_fd(const socket_type type, const std::string& address, const std::uint16_t port);
}
