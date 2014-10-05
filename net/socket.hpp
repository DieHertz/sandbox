#pragma once

#include "socket_utils.hpp"
#include "exception.hpp"
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <vector>

namespace net {
    enum timeout_type : std::int32_t {
        timeout_indefinite = -1,
        timeout_none
    };

    class socket {
    public:
        explicit socket(const int socket_fd) : socket_fd{socket_fd} {}
        socket(const socket_type type, const std::uint16_t port) : socket{type, {}, port} {}
        socket(const socket_type type, const std::string& address, const std::uint16_t port)
            : socket_fd{create_socket_fd(type, address, port)} {}

        socket(socket&& other) { *this = std::move(other); }

        socket& operator=(socket&& other) {
            std::swap(socket_fd, other.socket_fd);

            return *this;
        }

        ~socket() {
            if (!is_valid()) return;

            shutdown(socket_fd, SHUT_RDWR);
            close(socket_fd);
        }

        socket accept() {
            sockaddr_storage addr;
            socklen_t addr_size{sizeof(addr)};

            const auto client_socket_fd = ::accept(socket_fd, reinterpret_cast<sockaddr*>(&addr), &addr_size);
            if (client_socket_fd == -1) throw errno_exception{};

            return socket{client_socket_fd};
        }

        int get_fd() const { return socket_fd; }

        std::uint32_t read(std::vector<char>& buffer, const std::uint32_t num_bytes, const std::uint32_t offset) {
            const auto bytes_received = recv(socket_fd, buffer.data() + offset, num_bytes, 0);
            if (bytes_received == -1) throw errno_exception{};
            else if (bytes_received == 0 && num_bytes != 0) throw connection_closed{};

            return bytes_received;
        }

        std::vector<char> read(const std::uint32_t num_bytes) {
            std::vector<char> buffer(num_bytes);

            const auto bytes_read = read(buffer, num_bytes, 0);
            buffer.resize(bytes_read);

            return buffer;
        }

        void write(const std::vector<char>& buffer, const std::uint32_t num_bytes, const std::uint32_t offset) {
            std::uint32_t total_bytes_sent{};

            while (total_bytes_sent < num_bytes) {
                const auto bytes_sent = send(socket_fd, buffer.data() + offset, num_bytes - total_bytes_sent, 0);
                if (bytes_sent == -1) throw errno_exception{};

                total_bytes_sent += bytes_sent;
            }
        }

        void write(const std::vector<char>& buffer) {
            write(buffer, buffer.size(), 0);
        }

        bool is_data_available(const std::int32_t timeout_msec = timeout_indefinite) const {
            if (!is_valid()) return false;

            fd_set read_fds{};

            FD_ZERO(&read_fds);
            FD_SET(socket_fd, &read_fds);

            if (timeout_msec == -1) {
                select(socket_fd + 1, &read_fds, nullptr, nullptr, nullptr);
            } else {
                auto tv = get_timeval(timeout_msec);
                select(socket_fd + 1, &read_fds, nullptr, nullptr, &tv);
            }

            return FD_ISSET(socket_fd, &read_fds);
        }

        bool is_valid() const { return socket_fd != -1; }

    private:
        static timeval get_timeval(const std::int32_t msec) {
            return { msec / 1000, 0 };
        }

        int socket_fd{-1};
    };
}
