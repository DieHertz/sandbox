#pragma once

#include "socket.hpp"
#include "socket_streambuf.hpp"
#include <iostream>
#include <memory>

namespace net {
    class socket_stream : public std::iostream {
        std::unique_ptr<std::streambuf> streambuf_ptr;

    public:
        socket_stream(socket& s) : std::iostream{new socket_streambuf{s}}, streambuf_ptr{rdbuf()} {}
    };
}
