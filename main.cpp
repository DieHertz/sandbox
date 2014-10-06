#include <net/tcp/socket.hpp>
#include <net/tcp/socket_stream.hpp>
#include <ext/scope_exit.hpp>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> quit{};

void client_bot_function(const std::string& address, const std::uint16_t port) {
    net::tcp::socket socket{net::tcp::socket_type::client, address, port};
    net::tcp::socket_stream stream{socket};

    while (!quit.load(std::memory_order_relaxed)) {
        stream << "ping" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}

void client_handler_function(net::tcp::socket socket) {
    net::tcp::socket_stream stream{socket};

    std::cout << "client " << socket.get_fd() << " has connected" << std::endl;

    std::string received_string;
    while (!quit.load(std::memory_order_relaxed) &&
           std::getline(stream, received_string)) {
        std::cout << "client " << socket.get_fd() << " : " << received_string << std::endl;

        if (received_string.find("ping") == 0) {
            stream << "pong" << std::endl;
        } else {
            stream << received_string << std::endl;
        }

        if (received_string.find("exit") == 0) {
            quit.store(true, std::memory_order_relaxed);
        };
    }

    std::cout << "client " << socket.get_fd() << " has disconnected" << std::endl;
}

int main(int argc, char** argv) {
    try {
        if (argc != 2 && argc != 3) throw std::invalid_argument{"no port and host supplied"};

        const std::uint16_t port = std::stoi(argv[1]);
        const auto address = argc == 3 ? argv[2] : std::string{};
        net::tcp::socket server_socket{net::tcp::socket_type::server, address, port};

        std::vector<std::thread> threads{};
        scope_exit({
            for (auto& thread : threads) thread.join();
        });

        threads.emplace_back(client_bot_function, std::cref(address), port);

        while (!quit.load(std::memory_order_relaxed)) {
            net::tcp::socket client_socket{};

            if (server_socket.accept_non_blocking(client_socket, 100)) {
                threads.emplace_back(client_handler_function, std::move(client_socket));
            }
        }

        std::cout << "quit requested" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "unexpected exception" << std::endl;
    }
}
