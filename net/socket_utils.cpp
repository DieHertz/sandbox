#include "socket_utils.hpp"
#include "get_addrinfo.hpp"
#include "exception.hpp"
#include <sys/time.h>
#include <sys/socket.h>

namespace net {
    namespace {
        void connect_socket(const int socket_fd, const addrinfo_ptr_t& info) {
            const auto connect_result = connect(socket_fd, info->ai_addr, info->ai_addrlen);
            if (connect_result == -1) throw errno_exception{};
        }

        void bind_listen_socket(const int socket_fd, const addrinfo_ptr_t& info, const int backlog = int{}) {
            const auto bind_result = bind(socket_fd, info->ai_addr, info->ai_addrlen);
            if (bind_result == -1) throw errno_exception{};

            const auto listen_result = listen(socket_fd, backlog);
            if (listen_result == -1) throw errno_exception{};
        }
    }

    int create_socket_fd(const socket_type type, const std::string& address, const std::uint16_t port) {
        const auto info = get_addrinfo(address, std::to_string(port));

        const auto socket_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (socket_fd == -1) throw errno_exception{};

        switch (type) {
        case socket_type::client: connect_socket(socket_fd, info); break;
        case socket_type::server: bind_listen_socket(socket_fd, info); break;
        default: throw std::invalid_argument{"invalid socket_type"};
        }

        return socket_fd;
    }

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
