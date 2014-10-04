#pragma once

#include "exception.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <string>

namespace net {
    class socket {
    public:
        explicit socket(const int socket_fd) : socket_fd{socket_fd} {}
        socket(const std::string& address, const std::uint16_t port)
            : socket_fd{create_socket(address, port)} {
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
            socklen_t addr_size;
            const auto client_socket_fd = ::accept(socket_fd, reinterpret_cast<sockaddr*>(&addr), &addr_size);
            if (client_socket_fd == -1) throw errno_exception{};

            return socket{client_socket_fd};
        }

        int get_fd() const { return socket_fd; }

    private:
        static int create_socket(const std::string& address, const std::uint16_t port) {
            sockaddr_in addr{AF_INET, htons(port)};

            const auto result = inet_pton(AF_INET, address.data(), &addr.sin_addr);
            if (result == 0) throw invalid_address{address};
            else if (result == -1) throw errno_exception{};

            const auto socket_fd = ::socket(PF_INET, SOCK_STREAM, 0);
            if (socket_fd == -1) throw errno_exception{};

            const auto bind_result = bind(socket_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
            if (bind_result == -1) throw errno_exception{};

            const auto listen_result = listen(socket_fd, 10);
            if (listen_result == -1) throw errno_exception{};

            return socket_fd;
        }

        int socket_fd{};
    };
}
