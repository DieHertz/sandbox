#include "net/socket.hpp"
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
    if (argc != 3) throw std::invalid_argument{"no host and port supplied"};

    const auto address = argv[1];
    const std::uint16_t port = std::stoi(argv[2]);
    net::socket server_socket{address, port};

    while (true) {
        auto client_socket = server_socket.accept();

        std::thread{[] (net::socket client_socket) {
            try {
                const auto buffer_size = 4096;
                char buffer[buffer_size];

                while (true) {
                    const auto bytes_received = recv(client_socket.get_fd(), buffer, buffer_size, 0);
                    if (bytes_received == -1) throw net::errno_exception{};

                    int total_bytes_sent{};
                    while (total_bytes_sent < bytes_received) {
                        const auto bytes_sent = send(client_socket.get_fd(), buffer + total_bytes_sent, bytes_received - total_bytes_sent, 0);
                        if (bytes_sent == -1) throw net::errno_exception{};

                        total_bytes_sent += bytes_sent;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }, std::move(client_socket)}.detach();
    }
}
