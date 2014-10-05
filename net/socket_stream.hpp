#pragma once

#include "socket.hpp"
#include "socket_streambuf.hpp"
#include <iostream>

namespace net {
    class socket_stream : public std::iostream {
    public:
        socket_stream(socket& s) : std::iostream{new socket_streambuf{s}} {}
    };
}
