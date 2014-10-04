#pragma once

#include "get_addrinfo.hpp"
#include "exception.hpp"
#include <sys/socket.h>
#include <cstdint>
#include <string>

namespace net {
    enum struct socket_type {
        client,
        server
    };

    inline void connect_socket(const int socket_fd, const addrinfo_ptr_t& info) {
        const auto connect_result = connect(socket_fd, info->ai_addr, info->ai_addrlen);
        if (connect_result == -1) throw errno_exception{};
    }

    inline void bind_listen_socket(const int socket_fd, const addrinfo_ptr_t& info, const int backlog = int{}) {
        const auto bind_result = bind(socket_fd, info->ai_addr, info->ai_addrlen);
        if (bind_result == -1) throw errno_exception{};

        const auto listen_result = listen(socket_fd, backlog);
        if (listen_result == -1) throw errno_exception{};
    }

    inline int create_socket_fd(const socket_type type, const std::string& address, const std::uint16_t port) {
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
