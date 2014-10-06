#include "socket_utils.hpp"
#include "exception.hpp"
#include <sys/time.h>

namespace net {
    bool is_ready(const int socket_fd, const select_type type, const std::int32_t timeout_msec) {
        fd_set fds{};
        FD_ZERO(&fds);
        FD_SET(socket_fd, &fds);

        fd_set* read_fds{};
        fd_set* write_fds{};

        switch (type) {
        case select_type::read: read_fds = &fds; break;
        case select_type::write: write_fds = &fds; break;
        default: throw std::invalid_argument{"invalid select_type"};
        }

        timeval timeout{timeout_msec / 1000, 0};

        const auto result = select(socket_fd + 1, read_fds, write_fds, nullptr, &timeout);
        if (result == -1) throw errno_exception{};
        else if (result == 0) return false;

        return FD_ISSET(socket_fd, &fds);
    }
}
