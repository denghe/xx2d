#pragma once
#include "pch.h"
#include <random>

// reference from https://github.com/cslarsen/mersenne-twister
// faster than std impl, can store & restore state data directly
// ser/de data size == 5000 bytes
struct Rnd {

#pragma region impl
    inline static const size_t SIZE = 624;
    inline static const size_t PERIOD = 397;
    inline static const size_t DIFF = SIZE - PERIOD;
    inline static const uint32_t MAGIC = 0x9908b0df;

    uint32_t MT[SIZE];
    uint32_t MT_TEMPERED[SIZE];
    size_t index = SIZE;
    uint32_t seed;

#define Random5_M32(x) (0x80000000 & x) // 32nd MSB
#define Random5_L31(x) (0x7FFFFFFF & x) // 31 LSBs
#define Random5_UNROLL(expr) \
  y = Random5_M32(MT[i]) | Random5_L31(MT[i+1]); \
  MT[i] = MT[expr] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC); \
  ++i;
    void Generate() {
        size_t i = 0;
        uint32_t y;
        while (i < DIFF) {
            Random5_UNROLL(i + PERIOD);
        }
        while (i < SIZE - 1) {
            Random5_UNROLL(i - DIFF);
        }
        {
            y = Random5_M32(MT[SIZE - 1]) | Random5_L31(MT[0]);
            MT[SIZE - 1] = MT[PERIOD - 1] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC);
        }
        for (size_t i = 0; i < SIZE; ++i) {
            y = MT[i];
            y ^= y >> 11;
            y ^= y << 7 & 0x9d2c5680;
            y ^= y << 15 & 0xefc60000;
            y ^= y >> 18;
            MT_TEMPERED[i] = y;
        }
        index = 0;
    }
#undef Random5_UNROLL
#undef Random5_L31
#undef Random5_M32
#pragma endregion

    void SetSeed(uint32_t const& seed = std::random_device()()) {
        this->seed = seed;
        MT[0] = seed;
        index = SIZE;
        for (uint_fast32_t i = 1; i < SIZE; ++i) {
            MT[i] = 0x6c078965 * (MT[i - 1] ^ MT[i - 1] >> 30) + i;
        }
    }

    uint32_t Get() {
        if (index == SIZE) {
            Generate();
            index = 0;
        }
        return MT_TEMPERED[index++];
    }

    void NextBytes(void* buf, size_t len) {
        if (index == SIZE) {
            Generate();
            index = 0;
        }
        if (auto left = (SIZE - index) * 4; left >= len) {
            memcpy(buf, &MT_TEMPERED[index], len);
            index += len / 4 + (len % 4 ? 1 : 0);
        }
        else {
            memcpy(buf, &MT_TEMPERED[index], left);
            index = SIZE;
            NextBytes((char*)buf + left, len - left);
        }
    }

    template<typename V = int32_t, class = std::enable_if_t<std::is_arithmetic_v<V>>>
    V Next() {
        if constexpr (std::is_same_v<bool, std::decay_t<V>>) {
            return Get() >= std::numeric_limits<uint32_t>::max() / 2;
        }
        else if constexpr (std::is_integral_v<V>) {
            std::make_unsigned_t<V> v;
            if constexpr (sizeof(V) <= 4) {
                v = (V)Get();
            }
            else {
                v = (V)(Get() | ((uint64_t)Get() << 32));
            }
            if constexpr (std::is_signed_v<V>) {
                return (V)(v & std::numeric_limits<V>::max());
            }
            else return (V)v;
        }
        else if constexpr (std::is_floating_point_v<V>) {
            if constexpr (sizeof(V) == 4) {
                return (float)(double(Get()) / 0xFFFFFFFFu);
            }
            else if constexpr (sizeof(V) == 8) {
                constexpr auto max53 = (1ull << 53) - 1;
                auto v = ((uint64_t)Get() << 32) | Get();
                return double(v & max53) / max53;
            }
        }
        assert(false);
    }

    template<typename V>
    V Next(V from, V to) {
        assert(to >= from);
        if (from == to) return from;
        else {
            return from + Next<V>() % (to - from + 1);
        }
    }

    template<typename V>
    V Next(V to) {
        return Next(0, to);
    }
};
