#pragma once
#include "xx_typehelpers.h"
#include "xx_math.h"
#include <cstring>

namespace xx {

	// 最基础的二进制数据容器. 两种模式: 1. 追加.  2. 只读引用	( 当 cap == -1 )
	struct Data {
		char*				buf = nullptr;
		size_t				len = 0;
		size_t				cap = 0;

		// buf 头部预留空间大小. 至少需要装得下 sizeof(size_t)
		static const size_t	recvLen = 16;
		static const size_t	_1 = (size_t)-1;

		Data() = default;

		// 预分配空间 构造
		explicit Data(size_t const& newCap) {
			if (newCap) {
				auto siz = Round2n(recvLen + newCap);
				buf = ((char*)::malloc(siz)) + recvLen;
				cap = siz - recvLen;
			}
		}

		// 通过 复制一段数据 来构造
		Data(char const* const& ptr, size_t const& siz) {
			WriteBuf(ptr, siz);
		}

		// 通过 初始化列表 来构造
		Data(std::initializer_list<char> bytes)	: Data( bytes.begin(), bytes.size() ){}

		// 复制构造
		Data(Data const& o) {
			operator=(o);
		}
		inline Data& operator=(Data const& o) {
			if (o.cap == _1) {
				buf = o.buf;
				len = o.len;
				cap = o.cap;
				++Refs();
			}
			else {
				Clear();
				WriteBuf(o.buf, o.len);
			}
			return *this;
		}

		// 移动构造
		Data(Data&& o)  noexcept {
			operator=(std::move(o));
		}
		inline Data& operator=(Data&& o)  noexcept {
			std::swap(buf, o.buf);
			std::swap(len, o.len);
			std::swap(cap, o.cap);
			return *this;
		}

		// 判断数据是否一致
		inline bool operator==(Data const& o) {
			if (&o == this) return true;
			if (len != o.len) return false;
			return 0 == ::memcmp(buf, o.buf, len);
		}
		inline bool operator!=(Data const& o) {
			return !this->operator==(o);
		}

		// todo: 大小比较?

		// 确保空间足够
		inline void Reserve(size_t const& newCap) {
			assert(cap != _1);
			if (newCap <= cap) return;

			auto siz = Round2n(recvLen + newCap);
			auto newBuf = (char*)::malloc(siz) + recvLen;
			::memcpy(newBuf, buf, len);

			// 这里判断 cap 不判断 buf, 是因为 gcc 优化会导致 if 失效, 无论如何都会执行 free
			if (cap) {
				::free(buf - recvLen);
			}
			buf = newBuf;
			cap = siz - recvLen;
		}

		// 返回旧长度
		inline size_t Resize(size_t const& newLen) {
			assert(cap != _1);
			if (newLen > len) {
				Reserve(newLen);
			}
			auto rtv = len;
			len = newLen;
			return rtv;
		}

		// 下标访问
		inline char& operator[](size_t const& idx) {
			assert(idx < len);
			return buf[idx];
		}
		inline char const& operator[](size_t const& idx) const {
			assert(idx < len);
			return buf[idx];
		}


		// 从头部移除指定长度数据( 常见于拆包处理移除掉已经访问过的包数据, 将残留部分移动到头部 )
		inline void RemoveFront(size_t const& siz) {
			assert(cap != _1);
			assert(siz <= len);
			if (!siz) return;
			len -= siz;
			if (len) {
				::memmove(buf, buf + siz, len);
			}
		}

		// 追加写入一段 buf
		inline void WriteBuf(void const* const& ptr, size_t const& siz) {
			assert(cap != _1);
			Reserve(len + siz);
			::memcpy(buf + len, ptr, siz);
			len += siz;
		}
		
		// 追加写入一段 pod 结构内存
		template<typename T, typename ENABLED = std::enable_if_t<IsPod_v<T>>>
		void WriteFixed(T const& v) {
			WriteBuf(&v, sizeof(T));
		}

		// 追加写入整数( 7bit 变长格式 )
		template<typename T, bool needReserve = true, typename ENABLED = std::enable_if_t<std::is_integral_v<T>>>
		void WriteVarIntger(T const& v) {
			using UT = std::make_unsigned_t<T>;
			UT u(v);
			if constexpr (std::is_signed_v<T>) {
				u = ZigZagEncode(v);
			}
			if constexpr (needReserve) {
				Reserve(len + sizeof(T) + 1);
			}
			while (u >= 1 << 7) {
				buf[len++] = char((u & 0x7fu) | 0x80u);
				u = UT(u >> 7);
			};
			buf[len++] = char(u);
		}

		// 设置为只读模式, 并初始化引用计数( 开启只读引用计数模式. 没数据不允许开启 )
		inline void SetReadonlyMode() {
			assert(cap != _1);
			assert(len);
			cap = _1;
			Refs() = 1;
		}

		// 判断是否为只读模式
		inline bool Readonly() const {
			return cap == _1;
		}

		// 访问引用计数
		inline size_t& Refs() const {
			assert(cap == _1);
			return *(size_t*)(buf - recvLen);
		}

		// 引用模式减持, 追加模式释放 buf
		~Data() {
			if (cap == _1 && --Refs()) return;
			if (cap) {
				::free(buf - recvLen);
			}
		}

		// len 清 0, 可彻底释放 buf
		inline void Clear(bool const& freeBuf = false) {
			assert(cap != _1);
			if (freeBuf && cap) {
				::free(buf - recvLen);
				buf = nullptr;
				cap = 0;
			}
			len = 0;
		}
	};

	template<>
	struct IsPod<Data, void> : std::true_type {};
}
