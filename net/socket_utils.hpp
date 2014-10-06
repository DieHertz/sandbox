#pragma once

#include <cstdint>

namespace net {
    enum struct select_type {
        read,
        write,
    };
    bool is_ready(int socket_fd, select_type type, std::int32_t timeout_msec);
}
