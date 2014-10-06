#pragma once

#include "socket_type.hpp"
#include <cstdint>
#include <string>

namespace net {
    namespace tcp {
        int create_socket(socket_type type, const std::string& address, const std::uint16_t port);
    }
}
