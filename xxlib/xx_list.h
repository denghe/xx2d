#pragma once
#include "xx_helpers.h"

namespace xx {

	// std::vector similar, resize no fill zero, move only, Shared/Weak faster memcpy
	template<typename T, typename SizeType = ptrdiff_t>
	struct List {
		typedef T ChildType;
		T* buf{};
		SizeType cap{}, len{};

		List() = default;
		List(List const& o) = delete;
		List& operator=(List const& o) = delete;
		List(List&& o) noexcept
			: buf(o.buf)
			, cap(o.cap)
			, len(o.len) {
			o.buf = nullptr;
			o.cap = 0;
			o.len = 0;
		}
		List& operator=(List&& o) noexcept {
			std::swap(buf, o.buf);
			std::swap(len, o.len);
			std::swap(cap, o.cap);
			return *this;
		}
		~List() noexcept {
			Clear(true);
		}

		void Reserve(SizeType const& cap_) noexcept {
			assert(cap_ > 0);
			if (cap_ <= cap) return;
			if (!cap) {
				buf = (T*)::malloc(cap_ * sizeof(T));
				cap = cap_;
				return;
			}
			do {
				cap *= 2;
			} while (cap < cap_);
			auto newBuf = (T*)::malloc(cap * sizeof(T));

			if constexpr (IsPod_v<T>) {
				::memcpy((void*)newBuf, (void*)buf, len * sizeof(T));
			}
			else {
				for (SizeType i = 0; i < len; ++i) {
					new (&newBuf[i]) T((T&&)buf[i]);
					buf[i].~T();
				}
			}
			::free(buf);
			buf = newBuf;
		}

		void Resize(SizeType const& len_) noexcept {
			if (len_ == len) return len_;
			else if (len_ < len) {
				for (SizeType i = len_; i < len; ++i) {
					buf[i].~T();
				}
			}
			else {	// len_ > len
				Reserve(len_);
				if constexpr (!std::is_pod_v<T>) {
					for (SizeType i = this->len; i < len_; ++i) {
						new (buf + i) T();
					}
				}
			}
			len = len_;
		}

		T const& operator[](SizeType const& idx) const noexcept {
			assert(idx < len);
			return buf[idx];
		}
		T& operator[](SizeType const& idx) noexcept {
			assert(idx < len);
			return buf[idx];
		}
		T const& At(SizeType const& idx) const noexcept {
			assert(idx < len);
			return buf[idx];
		}
		T& At(SizeType const& idx) noexcept {
			assert(idx < len);
			return buf[idx];
		}

		T& Top() noexcept {
			assert(len > 0);
			return buf[len - 1];
		}
		void Pop() noexcept {
			assert(len > 0);
			--len;
			buf[len].~T();
		}
		T const& Top() const noexcept {
			assert(len > 0);
			return buf[len - 1];
		}
		bool TryPop(T& output) noexcept {
			if (!len) return false;
			output = (T&&)buf[--len];
			buf[len].~T();
			return true;
		}

		void Clear(bool const& freeBuf = false) noexcept {
			if (!cap) return;
			if (len) {
				for (SizeType i = 0; i < len; ++i) {
					buf[i].~T();
				}
				len = 0;
			}
			if (freeBuf) {
				::free(buf);
				buf = {};
				cap = 0;
			}
		}

		void Remove(T const& v) noexcept {
			for (SizeType i = 0; i < len; ++i) {
				if (v == buf[i]) {
					RemoveAt(i);
					return;
				}
			}
		}

		void RemoveAt(SizeType const& idx) noexcept {
			assert(idx < len);
			--len;
			if constexpr (IsPod_v<T>) {
				buf[idx].~T();
				::memmove(buf + idx, buf + idx + 1, (len - idx) * sizeof(T));
			}
			else {
				for (SizeType i = idx; i < len; ++i) {
					buf[i] = (T&&)buf[i + 1];
				}
				buf[len].~T();
			}
		}

		// �� T ��һ��������캯���Ĳ�����׷�ӹ���һ�� item
		template<typename...Args>
		T& Emplace(Args&&...args) noexcept {
			Reserve(len + 1);
			return *new (&buf[len++]) T(std::forward<Args>(args)...);
		}

		// �� Emplace ��ͬ����, �����֧��1�������Ĺ��캯��, ��һ��׷�Ӷ��
		template<typename ...TS>
		void Add(TS&&...vs) noexcept {
			std::initializer_list<int> n{ (Emplace(std::forward<TS>(vs)), 0)... };
			(void)(n);
		}

		void AddRange(T const* const& items, SizeType const& count) noexcept {
			Reserve(len + count);
			if constexpr (IsPod_v<T>) {
				::memcpy(buf + len, items, count * sizeof(T));
			}
			else {
				for (SizeType i = 0; i < count; ++i) {
					new (&buf[len + i]) T((T&&)items[i]);
				}
			}
			len += count;
		}

		template<typename T2>
		void AddRange(List<T2> const& list) noexcept {
			return AddRange(list.buf, list.len);
		}

		// ����ҵ��ͷ�������. �Ҳ��������� SizeType(-1)
		SizeType Find(T const& v) const noexcept {
			for (SizeType i = 0; i < len; ++i) {
				if (v == buf[i]) return i;
			}
			return SizeType(-1);
		}

		// ������ڷ��������ľͷ��� true
		bool Exists(std::function<bool(T const& v)>&& cond) const noexcept {
			if (!cond) return false;
			for (SizeType i = 0; i < len; ++i) {
				if (cond(buf[i])) return true;
			}
			return false;
		}

		// ��֧�� for( auto&& c : list ) �﷨.
		struct Iter {
			T* ptr;
			bool operator!=(Iter const& other) noexcept { return ptr != other.ptr; }
			Iter& operator++() noexcept { ++ptr; return *this; }
			T& operator*() noexcept { return *ptr; }
		};
		Iter begin() noexcept { return Iter{ buf }; }
		Iter end() noexcept { return Iter{ buf + len }; }
		Iter begin() const noexcept { return Iter{ buf }; }
		Iter end() const noexcept { return Iter{ buf + len }; }
	};

	// ��ʶ�ڴ���ƶ�
	template<typename T, typename SizeType>
	struct IsPod<List<T, SizeType>, void> : std::true_type {};
}
