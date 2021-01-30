#pragma once
#include "xx_fixeddata.h"
#include "xx_data.h"
#include <ctime>
#include <chrono>
#include <ostream>
#include <iomanip>
#include <cassert>

namespace xx {
    // 类型适配模板 for FixedData<size>::Write
    template<typename T, typename ENABLED = void>
    struct DumpFuncs;

    template<>
    struct DumpFuncs<char *> {
        static const char value = 0;

        inline static void Dump(std::ostream &o, char *&v) {
            o << std::string_view(v + sizeof(size_t), *(size_t *) v);
            v += sizeof(size_t) + *(size_t *) v;
        }
    };

    template<>
    struct DumpFuncs<bool> {
        static const char value = 1;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(bool *) v;
            v += sizeof(bool);
        }
    };

    template<>
    struct DumpFuncs<char> {
        static const char value = 2;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *v;
            v += sizeof(char);
        }
    };

    template<>
    struct DumpFuncs<short> {
        static const char value = 3;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(short *) v;
            v += sizeof(short);
        }
    };

    template<>
    struct DumpFuncs<int> {
        static const char value = 4;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(int *) v;
            v += sizeof(int);
        }
    };

    template<>
    struct DumpFuncs<long long> {
        static const char value = 5;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(long long *) v;
            v += sizeof(long long);
        }
    };

    template<>
    struct DumpFuncs<float> {
        static const char value = 6;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(float *) v;
            v += sizeof(float);
        }
    };

    template<>
    struct DumpFuncs<double> {
        static const char value = 7;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(double *) v;
            v += sizeof(double);
        }
    };

    template<>
    struct DumpFuncs<unsigned char> {
        static const char value = 8;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(unsigned char *) v;
            v += sizeof(unsigned char);
        }
    };

    template<>
    struct DumpFuncs<unsigned short> {
        static const char value = 9;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(unsigned short *) v;
            v += sizeof(unsigned short);
        }
    };

    template<>
    struct DumpFuncs<unsigned int> {
        static const char value = 10;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(unsigned int *) v;
            v += sizeof(unsigned int);
        }
    };

    template<>
    struct DumpFuncs<unsigned long long> {
        static const char value = 11;

        inline static void Dump(std::ostream &o, char *&v) {
            o << *(unsigned long long *) v;
            v += sizeof(unsigned long long);
        }
    };

    template<>
    struct DumpFuncs<std::chrono::system_clock::time_point> {
        static const char value = 12;

        inline static void Dump(std::ostream &o, char *&v) {
            auto&& t = std::chrono::system_clock::to_time_t(*(std::chrono::system_clock::time_point*)v);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            o << std::put_time(&tm, "%F %T");
            v += sizeof(std::chrono::system_clock::time_point);
        }
    };

    template<>
    struct DumpFuncs<DataView> {
        static const char value = 13;

        inline static void Dump(std::ostream &o, char *&v) {
            auto len = *(size_t *)v;
            auto buf = v + sizeof(len);
            o.put('[');
            if (len) {
                for(size_t i = 0; i < len; ++i) {
                    o << (int)(uint8_t)buf[i];
                    if(i<len-1) {
                        o.put(',');
                    }
                }
            }
            o.put(']');
            v += sizeof(size_t) + len;
        }
    };


    // 适配模板 for FixedData<size>::Write
    template<size_t size, typename T, typename ENABLED = void>
    struct BufFuncs {
        static inline void Write(FixedData<size> &data, T const &in) {
            // 不支持的数据类型
            assert(false);
        }
    };

    // 适配所有数字类型
    template<size_t size, typename T>
    struct BufFuncs<size, T, std::enable_if_t<std::is_arithmetic_v<T>>> {
        // 1 byte typeId + data
        static inline void Write(FixedData<size> &data, T const &in) {
            data.Ensure(1 + sizeof(T));
            if constexpr(std::is_same_v<bool, T>) {
                data.buf[data.len] = DumpFuncs<bool>::value;
            }
            else if constexpr(std::is_floating_point_v<T>) {
                if constexpr(sizeof(T) == 4) data.buf[data.len] = DumpFuncs<float>::value;
                else if constexpr(sizeof(T) == 8) data.buf[data.len] = DumpFuncs<double>::value;
                else {
                    static_assert(true, "unsupported data type.");
                }
            }
            else if constexpr(std::is_signed_v<T>) {
                if constexpr(sizeof(T) == 1) data.buf[data.len] = DumpFuncs<char>::value;
                else if constexpr(sizeof(T) == 2) data.buf[data.len] = DumpFuncs<short>::value;
                else if constexpr(sizeof(T) == 4) data.buf[data.len] = DumpFuncs<int>::value;
                else if constexpr(sizeof(T) == 8) data.buf[data.len] = DumpFuncs<long long>::value;
                else {
                    static_assert(true, "unsupported data type.");
                }
            }
            else {
                if constexpr(sizeof(T) == 1) data.buf[data.len] = DumpFuncs<unsigned char>::value;
                else if constexpr(sizeof(T) == 2) data.buf[data.len] = DumpFuncs<unsigned short>::value;
                else if constexpr(sizeof(T) == 4) data.buf[data.len] = DumpFuncs<unsigned int>::value;
                else if constexpr(sizeof(T) == 8) data.buf[data.len] = DumpFuncs<unsigned long long>::value;
                else {
                    static_assert(true, "unsupported data type.");
                }
            }
            memcpy(data.buf + data.len + 1, &in, sizeof(T));
            data.len += 1 + sizeof(T);
        }
    };
	
	// 适配所有枚举( 转为数字调用上面的函数 )
    template<size_t size, typename T>
    struct BufFuncs<size, T, std::enable_if_t<std::is_enum_v<T>>> {
        typedef std::underlying_type_t<T> UT;
        static inline void Write(FixedData<size> &data, T const &in) {
            BufFuncs<size, UT ,void>::Write((UT)in);
        }
    };

    // 适配 pair<char*, len>
    template<size_t size>
    struct BufFuncs<size, std::pair<char *, size_t>> {
        // 1 byte typeId + len + data
        static inline void Write(FixedData<size> &data, std::pair<char *, size_t> const &in) {
            data.Ensure(1 + sizeof(in.second) + in.second);
            data.buf[data.len] = DumpFuncs<char *>::value;
            memcpy(data.buf + data.len + 1, &in.second, sizeof(in.second));
            memcpy(data.buf + data.len + 1 + sizeof(in.second), in.first, in.second);
            data.len += 1 + sizeof(in.second) + in.second;
        }
    };

    // 适配 literal char[len] string  ( 通常要去掉最后一个 0 不写 )
    template<size_t size, size_t len>
    struct BufFuncs<size, char[len], void> {
        static inline void Write(FixedData<size> &data, char const(&in)[len]) {
            BufFuncs<size, std::pair<char *, size_t>>::Write(data, {(char *) in, len - 1});
        }
    };

    // 适配 char const* \0 结尾 字串
    template<size_t size>
    struct BufFuncs<size, char const *, void> {
        static inline void Write(FixedData<size> &data, char const *const &in) {
            BufFuncs<size, std::pair<char *, size_t>>::Write(data, {(char*)in, strlen(in)});
        }
    };

    // 适配 char* \0 结尾 字串
    template<size_t size>
    struct BufFuncs<size, char *, void> {
        static inline void Write(FixedData<size> &data, char *const &in) {
            BufFuncs<size, std::pair<char *, size_t>>::Write(data, {in, strlen(in)});
        }
    };

    // 适配 std::string
    template<size_t size>
    struct BufFuncs<size, std::string, void> {
        static inline void Write(FixedData<size> &data, std::string const &in) {
            BufFuncs<size, std::pair<char *, size_t>>::Write(data, {(char*)in.data(), in.size()});
        }
    };

    // 适配 std::chrono::system_clock::time_point
    template<size_t size>
    struct BufFuncs<size, std::chrono::system_clock::time_point, void> {
        static inline void Write(FixedData<size> &data, std::chrono::system_clock::time_point const &in) {
            data.Ensure(1 + sizeof(std::chrono::system_clock::time_point));
            data.buf[data.len] = DumpFuncs<std::chrono::system_clock::time_point>::value;
            memcpy(data.buf + data.len + 1, &in, sizeof(std::chrono::system_clock::time_point));
            data.len += 1 + sizeof(std::chrono::system_clock::time_point);
        }
    };

    // 适配 xx::Data / xx::DataView
    template<size_t size, typename T>
    struct BufFuncs<size, T, std::enable_if_t<std::is_same_v<Data, T> || std::is_same_v<DataView, T>>> {
        static inline void Write(FixedData<size> &data, T const &in) {
            data.Ensure(1 + sizeof(in.len) + in.len);
            data.buf[data.len] = DumpFuncs<DataView>::value;
            memcpy(data.buf + data.len + 1, &in.len, sizeof(in.len));
            memcpy(data.buf + data.len + 1 + sizeof(in.len), in.buf, in.len);
            data.len += 1 + sizeof(in.len) + in.len;
        }
    };


    /*************************************************************************************/

    template<size_t size, typename ...TS>
    void WriteTo(FixedData<size> &data, TS const &...vs) {
        (BufFuncs<size, TS>::Write(data, vs), ...);
    }

    typedef void (*DumpFunc)(std::ostream &o, char *&v);

    // 自写扩展时用的起始 index
    static const int dumpFuncsSafeIndex = 14;

    inline DumpFunc dumpFuncs[] = {
            DumpFuncs<char *>::Dump,
            DumpFuncs<bool>::Dump,
            DumpFuncs<char>::Dump,
            DumpFuncs<short>::Dump,
            DumpFuncs<int>::Dump,
            DumpFuncs<long long>::Dump,
            DumpFuncs<float>::Dump,
            DumpFuncs<double>::Dump,
            DumpFuncs<unsigned char>::Dump,
            DumpFuncs<unsigned short>::Dump,
            DumpFuncs<unsigned int>::Dump,
            DumpFuncs<unsigned long long>::Dump,
            DumpFuncs<std::chrono::system_clock::time_point>::Dump,
            DumpFuncs<DataView>::Dump,
            // 占位符. 方便扩展
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
    };

    template<typename OS, size_t size>
    void DumpTo(OS &tar, FixedData<size> const &v, size_t const& bufOffset = 0) {
        auto begin = v.buf + bufOffset;
        auto end = v.buf + v.len;
        while (begin < end) {
            auto typeId = (int) *begin;
            ++begin;
            dumpFuncs[typeId](tar, begin);
        }
    }
}
