#pragma once
#include "xx_typehelpers.h"
#include "xx_math.h"
#include <cstring>

namespace xx {

	// 最基础的二进制数据容器. 两种模式: 1. 追加.  2. 只读引用	( 当 cap == -1 )
	struct Data {
		size_t				len = 0;
		size_t				cap = 0;
		size_t				offset = 0;
		char* buf = nullptr;

		// buf 头部预留空间大小. 至少需要装得下 sizeof(size_t)
		static const size_t	recvLen = 16;
		static const size_t	_1 = (size_t)-1;

		Data() = default;

		// 预分配空间 构造
		explicit Data(size_t const& newCap) {
			if (newCap) {
				auto siz = Round2n(recvLen + newCap);
				buf = (char*)::malloc(siz) + recvLen;
				cap = siz - recvLen;
			}
		}

		// 通过 复制一段数据 来构造
		Data(void const* const& ptr, size_t const& siz) {
			WriteBuf(ptr, siz);
		}

		// 复制构造
		Data(Data const& o) {
			operator=(o);
		}

		XX_FORCEINLINE Data& operator=(Data const& o) {
			if (o.cap == _1) {
				len = o.len;
				cap = o.cap;
				offset = o.offset;
				buf = o.buf;
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

		XX_FORCEINLINE Data& operator=(Data&& o)  noexcept {
			std::swap(len, o.len);
			std::swap(cap, o.cap);
			std::swap(offset, o.offset);
			std::swap(buf, o.buf);
			return *this;
		}

		// 判断数据是否一致
		XX_FORCEINLINE bool operator==(Data const& o) {
			if (&o == this) return true;
			if (len != o.len) return false;
			return 0 == ::memcmp(buf, o.buf, len);
		}

		XX_FORCEINLINE bool operator!=(Data const& o) {
			return !this->operator==(o);
		}

		// 确保空间足够
		template<bool CheckCap = true>
		XX_NOINLINE void Reserve(size_t const& newCap) {
			assert(cap != _1);
			if (CheckCap && newCap <= cap) return;

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
		XX_FORCEINLINE size_t Resize(size_t const& newLen) {
			assert(cap != _1);
			if (newLen > len) {
				Reserve<false>(newLen);
			}
			auto rtv = len;
			len = newLen;
			return rtv;
		}

		// 下标访问
		XX_FORCEINLINE char& operator[](size_t const& idx) {
			assert(idx < len);
			return buf[idx];
		}

		XX_FORCEINLINE char const& operator[](size_t const& idx) const {
			assert(idx < len);
			return buf[idx];
		}


		// 从头部移除指定长度数据( 常见于拆包处理移除掉已经访问过的包数据, 将残留部分移动到头部 )
		XX_FORCEINLINE void RemoveFront(size_t const& siz) {
			assert(cap != _1);
			assert(siz <= len);
			if (!siz) return;
			len -= siz;
			if (len) {
				::memmove(buf, buf + siz, len);
			}
		}

		// 通过 初始化列表 填充内容. 填充前会先 Clear
		template<typename T>
		XX_FORCEINLINE void Fill(std::initializer_list<T> const& bytes) {
			static_assert(std::is_integral_v<T>);
			Clear();
			Reserve(bytes.size());
			for (auto&& b : bytes) {
				buf[len++] = (uint8_t)b;
			}
		}

		// 追加写入一段 buf
		template<bool needReserve = true>
		XX_FORCEINLINE void WriteBuf(void const* const& ptr, size_t const& siz) {
			assert(cap != _1);
			if constexpr (needReserve) {
				if (len + siz > cap) {
					Reserve<false>(len + siz);
				}
			}
			::memcpy(buf + len, ptr, siz);
			len += siz;
		}

		// 追加写入一段 pod 结构内存
		template<typename T, bool needReserve = true, typename ENABLED = std::enable_if_t<IsPod_v<T>>>
		XX_FORCEINLINE void WriteFixed(T const& v) {
			assert(cap != _1);
			if constexpr (sizeof(T) == 1) {
				if constexpr (needReserve) {
					if (len + 1 > cap) {
						Reserve<false>(len + 1);
					}
				}
				buf[len++] = *(char*)&v;
			}
			else {
				WriteBuf<needReserve>(&v, sizeof(T));
			}
		}

		// 在指定 idx 写入定长数据
		template<typename T, typename ENABLED = std::enable_if_t<IsPod_v<T>>>
		XX_FORCEINLINE void WriteFixedAt(size_t const& idx, T const& v) {
			if (idx + sizeof(T) > len) {
				Resize(sizeof(T) + idx);
			}
			memcpy(buf + idx, &v, sizeof(T));
		}

		// 追加写入整数( 7bit 变长格式 )
		template<typename T, bool needReserve = true, typename ENABLED = std::enable_if_t<std::is_integral_v<T>>>
		XX_FORCEINLINE void WriteVarIntger(T const& v) {
			using UT = std::make_unsigned_t<T>;
			UT u(v);
			if constexpr (std::is_signed_v<T>) {
				u = ZigZagEncode(v);
			}
			if constexpr (needReserve) {
				if (len + sizeof(T) + 1 > cap) {
					Reserve<false>(len + sizeof(T) + 1);
				}
			}
			while (u >= 1 << 7) {
				buf[len++] = char((u & 0x7fu) | 0x80u);
				u = UT(u >> 7);
			};
			buf[len++] = char(u);
		}

		// 跳过指定长度字节数不写。返回起始 len
		template<bool needReserve = true>
		XX_FORCEINLINE size_t WriteJump(size_t const& siz) {
			assert(cap != _1);
			auto bak = len;
			if constexpr (needReserve) {
				if (len + siz > cap) {
					Reserve<false>(len + siz);
				}
			}
			len += siz;
			return bak;
		}


		// 读指定长度 buf 到 tar. 返回非 0 则读取失败
		// 用之前需要自己初始化 offset
		XX_FORCEINLINE int ReadBuf(char* const& tar, size_t const& siz) {
			if (offset + siz > len) return __LINE__;
			memcpy(tar, buf + offset, siz);
			offset += siz;
			return 0;
		}

		// 定长读. 返回非 0 则读取失败
		// 用之前需要自己初始化 offset
		template<typename T, typename ENABLED = std::enable_if_t<IsPod_v<T>>>
		XX_FORCEINLINE int ReadFixed(T& v) {
			if constexpr (sizeof(T) == 1) {
				if (offset == len) return __LINE__;
				v = *(T*)(buf + offset);
				++offset;
				return 0;
			}
			else {
				return ReadBuf((char*)&v, sizeof(T));
			}
		}

		// 从指定下标定长读. 不改变 offset. 返回非 0 则读取失败
		template<typename T, typename ENABLED = std::enable_if_t<IsPod_v<T>>>
		XX_FORCEINLINE int ReadFixedAt(size_t const& idx, T& v) {
			if (idx + sizeof(T) >= len) return __LINE__;
			if constexpr (sizeof(T) == 1) {
				v = *(T*)(buf + idx);
			}
			else {
				memcpy(&v, buf + idx, sizeof(T));
			}
			return 0;
		}

		// 变长读. 返回非 0 则读取失败
		// 用之前需要自己初始化 offset
		template<typename T>
		XX_FORCEINLINE int ReadVarInteger(T& v) {
			using UT = std::make_unsigned_t<T>;
			UT u(0);
			for (size_t shift = 0; shift < sizeof(T) * 8; shift += 7) {
				if (offset == len) return __LINE__;
				auto b = (UT)buf[offset++];
				u |= UT((b & 0x7Fu) << shift);
				if ((b & 0x80) == 0) {
					if constexpr (std::is_signed_v<T>) {
						v = ZigZagDecode(u);
					}
					else {
						v = u;
					}
					return 0;
				}
			}
			return __LINE__;
		}


		// 设置为只读模式, 并初始化引用计数( 开启只读引用计数模式. 没数据不允许开启 )
		XX_FORCEINLINE void SetReadonlyMode() {
			assert(cap != _1);
			assert(len);
			cap = _1;
			Refs() = 1;
		}

		// 判断是否为只读模式
		XX_FORCEINLINE bool Readonly() const {
			return cap == _1;
		}

		// 访问引用计数
		XX_FORCEINLINE size_t& Refs() const {
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
		XX_FORCEINLINE void Clear(bool const& freeBuf = false) {
			assert(cap != _1);
			if (freeBuf && cap) {
				::free(buf - recvLen);
				buf = nullptr;
				cap = 0;
			}
			len = 0;
			offset = 0;
		}
	};

	template<>
	struct IsPod<Data, void> : std::true_type {};



	struct DataView {
		char const* const& buf;
		size_t const& len;
	};
}
