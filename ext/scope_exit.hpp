#pragma once

#include <utility>

namespace ext {
    template<class Function> class __scope_exit {
        Function at_exit;

    public:
        __scope_exit(Function at_exit) : at_exit{at_exit} {}
        ~__scope_exit() { at_exit(); }
    };

    template<class Function>
    __scope_exit<Function> make_scope_exit(Function&& at_exit) {
        return std::forward<Function>(at_exit);
    }
}

#define scope_exit_concat(n, ...) \
auto at_scope_exit##n = ext::make_scope_exit([&] { __VA_ARGS__ })
#define scope_exit_fwd(n, ...) scope_exit_concat(n, __VA_ARGS__)
#define scope_exit(...) scope_exit_fwd(__LINE__, __VA_ARGS__)
