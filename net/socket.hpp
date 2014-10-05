#pragma once

#include "socket_utils.hpp"
#include "exception.hpp"
#include <sys/socket.h>
#include <cstdint>
#include <string>

namespace net {
    class socket {
    public:
        explicit socket(const int socket_fd) : socket_fd{socket_fd} {}
        socket(const socket_type type, const std::string& address, const std::uint16_t port)
            : socket_fd{create_socket_fd(type, address, port)} {
        }

        socket(socket&& other) : socket_fd{other.socket_fd} { other.socket_fd = 0; }
        socket& operator=(socket&& other) {
            std::swap(socket_fd, other.socket_fd);
            return *this;
        }

        ~socket() {
            shutdown(socket_fd, SHUT_RDWR);
        }

        socket accept() {
            sockaddr_storage addr;
            socklen_t addr_size{sizeof(addr)};

            const auto client_socket_fd = ::accept(socket_fd, reinterpret_cast<sockaddr*>(&addr), &addr_size);
            if (client_socket_fd == -1) throw errno_exception{};

            return socket{client_socket_fd};
        }

        int get_fd() const { return socket_fd; }

    private:
        int socket_fd{};
    };
}
