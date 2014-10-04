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
        std::cout << "client connected: " << client_socket.get_fd() << std::endl;

        std::thread{[] (net::socket client_socket) {
            try {
                std::string in;

                const auto buffer_size = 4096;
                char buffer[buffer_size];
                while (const auto bytes_received = recv(client_socket.get_fd(), buffer, buffer_size, 0)) {
                    if (bytes_received == -1) throw net::errno_exception{};

                    in.insert(std::end(in), buffer, buffer + bytes_received);
                }

                std::cout << in << std::endl;
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }, std::move(client_socket)}.detach();
    }
}
