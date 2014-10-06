#pragma once

#include "socket.hpp"
#include <streambuf>
#include <stdexcept>

namespace net {
namespace tcp {
    class socket_streambuf : public std::streambuf {
    public:
        explicit socket_streambuf(socket& s, const std::size_t buffer_size = 256, const std::size_t put_back = 8)
            : s(s), in_buffer(std::max(buffer_size, put_back) + put_back), put_back{put_back}, out_buffer(buffer_size + 1) {
            const auto in_end = &in_buffer.back() + 1;
            setg(in_end, in_end, in_end);

            setp(out_buffer.data(), out_buffer.data() + out_buffer.size() - 1);
        }

        socket_streambuf(const socket_streambuf&) = delete;
        socket_streambuf& operator=(const socket_streambuf&) = delete;

    private:
        int_type underflow() override {
            if (!(gptr() < egptr())) {
                const auto base = in_buffer.data();
                auto start = base;

                if (eback() == base) {
                    std::memmove(base, egptr() - put_back, put_back);
                    start += put_back;
                }

                const auto offset = start - base;
                const auto bytes_read = s.read(in_buffer, in_buffer.size() - offset, offset);

                if (bytes_read == 0) return traits_type::eof();

                setg(base, start, start + bytes_read);
            }

            return traits_type::to_int_type(*gptr());
        }

        int_type overflow(const int_type ch) override {
            if (ch == traits_type::eof()) return ch;

            if (!(pptr() <= epptr())) throw std::runtime_error{"put pointer points beyond end put pointer"};

            *pptr() = ch;
            pbump(1);

            send();

            return ch;
        }

        int sync() override {
            send();
            return 0;
        }

        void send() {
            const auto n = pptr() - pbase();
            s.write(pbase(), n);
            pbump(-n);
        }

        socket& s;
        std::vector<char> in_buffer;
        const std::size_t put_back;
        std::vector<char> out_buffer;
    };
}
}
