#include "net/socket.hpp"
#include "net/socket_stream.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void client_bot_function(const std::string& address, const std::uint16_t port) {
    net::socket socket{net::socket_type::client, address, port};
    net::socket_stream stream{socket};

    while (true) {
        stream << "ping" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}

void client_handler_function(net::socket socket) {
    net::socket_stream stream{socket};

    while (true) {
        std::string received_string;
        std::getline(stream, received_string);

        std::cout << "client " << socket.get_fd() << " : " << received_string << std::endl;

        stream << received_string << std::endl;

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
