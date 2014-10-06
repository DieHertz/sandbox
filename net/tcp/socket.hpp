#pragma once

#include "create_socket.hpp"
#include <net/socket_utils.hpp>
#include <net/exception.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <vector>

namespace net {
namespace tcp {
    enum timeout_type : std::int32_t {
        timeout_indefinite = -1,
        timeout_none
    };

    class socket {
    public:
        constexpr socket() = default;
        explicit socket(const int socket_fd) : socket_fd{socket_fd} {}
        socket(const socket_type type, const std::uint16_t port) : socket{type, {}, port} {}
        socket(const socket_type type, const std::string& address, const std::uint16_t port)
            : socket_fd{create_socket(type, address, port)} {}

        socket(socket&& other) { *this = std::move(other); }

        socket& operator=(socket&& other) {
            std::swap(socket_fd, other.socket_fd);

            return *this;
        }

        ~socket() {
            if (socket_fd == 0) return;

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

        bool accept_non_blocking(socket& s, const std::int32_t timeout_msec = 0) {
            if (!is_ready(socket_fd, select_type::read, timeout_msec)) return false;

            s = accept();
            return true;
        }

        int get_fd() const { return socket_fd; }

        std::uint32_t read(std::vector<char>& buffer, const std::uint32_t num_bytes, const std::uint32_t offset) {
            if (num_bytes == 0) throw std::invalid_argument{"num_bytes cannot be zero"};

            const auto bytes_received = recv(socket_fd, buffer.data() + offset, num_bytes, 0);
            if (bytes_received == -1) throw errno_exception{};
            else if (bytes_received == 0) throw connection_closed{};

            return bytes_received;
        }

        std::vector<char> read(const std::uint32_t num_bytes) {
            std::vector<char> buffer(num_bytes);

            const auto bytes_read = read(buffer, num_bytes, 0);
            buffer.resize(bytes_read);

            return buffer;
        }

        void write(const char* buffer, const std::uint32_t num_bytes) {
            std::uint32_t total_bytes_sent{};

            while (total_bytes_sent < num_bytes) {
                const auto bytes_sent = send(socket_fd, buffer, num_bytes - total_bytes_sent, 0);
                if (bytes_sent == -1) throw errno_exception{};

                total_bytes_sent += bytes_sent;
            }
        }

        void write(const std::vector<char>& buffer, const std::uint32_t num_bytes, const std::uint32_t offset) {
            write(buffer.data() + offset, num_bytes);
        }

        void write(const std::vector<char>& buffer) {
            write(buffer.data(), buffer.size());
        }

        bool is_valid() const { return socket_fd != 0; }

    private:
        int socket_fd{};
    };
}
}
