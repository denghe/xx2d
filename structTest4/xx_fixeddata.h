#pragma once
#include <cstring>
#include <cstdlib>

namespace xx {

    template<size_t size = 256>
    struct FixedData {
        char *buf;
        size_t len;
        size_t cap;
        static const size_t headerLen = sizeof(buf) + sizeof(len) + sizeof(cap);
        static const size_t innerSpaceLen = size - headerLen;
        char innerBuf[innerSpaceLen];

        FixedData() {
            buf = innerBuf;
            len = 0;
            cap = size - headerLen;
        }

        FixedData(FixedData &&o) noexcept {
            if (o.buf == o.innerBuf) {
                buf = innerBuf;
                memcpy(&len, &o.len, sizeof(len) + sizeof(cap) + o.len);
            } else {
                memcpy(this, &o, headerLen);
            }
            memset((void*)&o, 0, headerLen);
        }

        FixedData(FixedData const &o) = delete;

        FixedData &operator=(FixedData const &o) = delete;

        FixedData &operator=(FixedData &&o) = delete;

        void Ensure(size_t const &siz) {
            if (len + siz <= cap) return;
            while (cap < len + siz) {
                cap *= 2;
            }
            auto &&newBuf = (char *) malloc(cap);
            memcpy(newBuf, buf, len);
            if (buf != innerBuf) {
                free(buf);
            }
            buf = newBuf;
        }

        void Clear() {
            if (buf != innerBuf) {
                free(buf);
                buf = innerBuf;
                cap = size - headerLen;
            }
            len = 0;
        }

        ~FixedData() {
            if (buf != innerBuf) {
                free(buf);
            }
        }
    };
}
