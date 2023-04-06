#pragma once

// important: only support static function or lambda !!!  COPY data from arguments !!! do not ref !!!

#include "xx_listlink.h"

namespace cxx14 {}
namespace xx {
    using namespace std;
#if __has_include(<coroutine>)
#else
    using namespace std::experimental;
    using namespace cxx14;
#endif

    struct Coro {
        struct promise_type;
        using handle_type = coroutine_handle<promise_type>;
        struct promise_type {
            auto get_return_object() { return Coro(handle_type::from_promise(*this)); }
            suspend_never initial_suspend() { return {}; }
            suspend_always final_suspend() noexcept(true) { return {}; }
            suspend_always yield_value(int v) { return {}; }
            void return_void() {}
            void unhandled_exception() {
                std::rethrow_exception(std::current_exception());
            }
        };

        Coro(handle_type h) : h(h) {}
        ~Coro() { if (h) h.destroy(); }
        Coro(Coro const& o) = delete;
        Coro& operator=(Coro const&) = delete;
        Coro(Coro&& o) noexcept : h(o.h) { o.h = nullptr; };
        Coro& operator=(Coro&&) = delete;

        void operator()() { h.resume(); }
        operator bool() { return h.done(); }
        bool Resume() { h.resume(); return h.done(); }

    private:
        handle_type h;
    };

    struct Coros {
        Coros(Coros const&) = delete;
        Coros& operator=(Coros const&) = delete;
        Coros(Coros&&) = default;
        Coros& operator=(Coros&&) = default;
        Coros(int32_t cap = 8) {
            coros.Reserve(cap);
        }

        xx::ListLink<Coro, int32_t> coros;

        void Add(Coro&& g) {
            if (g) return;
            coros.Emplace(std::move(g));
        }

        void Clear() {
            coros.Clear();
        }

        bool operator()() {
            int prev = -1, next{};
            for (auto idx = coros.head; idx != -1;) {
                if (coros[idx].Resume()) {
                    next = coros.Remove(idx, prev);
                } else {
                    next = coros.Next(idx);
                    prev = idx;
                }
                idx = next;
            }
            return coros.Count();
        }

        int32_t Count() const {
            return coros.Count();
        }

        void Reserve(int32_t cap) {
            coros.Reserve(cap);
        }
    };
}

#define CoType xx::Coro
#define CoYield co_yield 0
#define CoReturn co_return
#define CoAwait( coType ) { auto&& c = coType; while(!c) { CoYield; c(); } }
#define CoSleep( duration ) { auto tp = std::chrono::steady_clock::now() + duration; do { CoYield; } while (std::chrono::steady_clock::now() < tp); }

/*

example:

#include "xx_coro_simple.h"
#include <thread>
#include <iostream>

CoType func1() {
    CoSleep(1s);
}
CoType func2() {
    CoAwait( func1() );
    std::cout << "func 1 out" << std::endl;
    CoAwait( func1() );
    std::cout << "func 1 out" << std::endl;
}

int main() {
    xx::Coros cs;

    auto func = [](int b, int e)->CoType {
        CoYield;
        for (size_t i = b; i <= e; i++) {
            std::cout << i << std::endl;
            CoYield;
        }
        std::cout << b << "-" << e << " end" << std::endl;
    };

    cs.Add(func(1, 5));
    cs.Add(func(6, 10));
    cs.Add(func(11, 15));
    cs.Add([]()->CoType {
        CoSleep(1s);
        std::cout << "CoSleep 1s" << std::endl;
    }());
    cs.Add(func2());

LabLoop:
    cs();
    if (cs) {
        std::this_thread::sleep_for(500ms);
        goto LabLoop;
    }

    return 0;
}

*/