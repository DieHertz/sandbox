#include "net/socket.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void client_bot_function(const std::string& address, const std::uint16_t port) {
    net::socket socket{net::socket_type::client, address, port};

    const auto data = "ping";

    while (const auto bytes_sent = send(socket.get_fd(), data, std::strlen(data), 0)) {
        if (bytes_sent == -1) throw net::errno_exception{};

        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }
}

void client_handler_function(net::socket socket) {
    const auto buffer_size = 4096;
    char buffer[buffer_size];

    while (true) {
        const auto bytes_received = recv(socket.get_fd(), buffer, buffer_size, 0);
        if (bytes_received == -1) throw net::errno_exception{};

        const std::string received_string{buffer, buffer + bytes_received};
        std::cout << "client " << socket.get_fd() << " : " << received_string << std::endl;

        int total_bytes_sent{};
        while (total_bytes_sent < bytes_received) {
            const auto bytes_sent = send(socket.get_fd(), buffer + total_bytes_sent, bytes_received - total_bytes_sent, 0);
            if (bytes_sent == -1) throw net::errno_exception{};

            total_bytes_sent += bytes_sent;
        }

        if (received_string.find("exit") == 0) throw net::exception{"exit requested"};
    }
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) throw std::invalid_argument{"no port and host supplied"};

    const std::uint16_t port = std::stoi(argv[1]);
    const auto address = argc == 3 ? argv[2] : std::string{};
    net::socket server_socket{net::socket_type::server, address, port};

    std::thread{client_bot_function, std::cref(address), port}.detach();

    while (true) {
        auto client_socket = server_socket.accept();
        std::thread{client_handler_function, std::move(client_socket)}.detach();
    }
}
