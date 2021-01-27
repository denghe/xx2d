#include <cstring>	// malloc realloc
//#include <cstddef>	// offsetof
#include <cassert>
#include <type_traits>

namespace xx {

	// 用来标识 LinkPool.buf 的成员类型 是否支持 realloc 扩容
	template<typename T, typename ENABLED = void> struct IsReallocable : std::false_type {};
	template<typename T> struct IsReallocable<T, std::enable_if_t<std::is_standard_layout_v<T>&& std::is_trivial_v<T>>> : std::true_type {};
	template<typename T> constexpr bool IsReallocable_v = IsReallocable<T>::value;


	// 链表节点池. 分配的是不会变化的 下标. 可指定 next 变量类型和偏移量
	template<typename T, typename PNType = int, size_t nextOffset = 0>
	class LinkPool {
		static_assert(sizeof(T) >= sizeof(int));
		static_assert(sizeof(T) >= sizeof(int) + nextOffset);

		T* buf = nullptr;			// 数组
		PNType len = 0;				// 已使用（并非已分配）对象个数
		PNType cap = 0;				// 数组长度
		PNType header = -1;			// 未分配节点单链表头下标
		PNType count = 0;			// 未分配节点单链表长度

		PNType& Next(T& o) {
			return *(PNType*)((char*)&o + nextOffset);
		}
	public:
		explicit LinkPool(PNType const& cap = 16) : cap(cap) {
			buf = (T*)malloc(sizeof(T) * cap);
		}
		~LinkPool() {
			Clear();
			free((void*)buf);
			buf = nullptr;
		}

		// 分配一个下标到 idx. 如果产生了 resize 行为，将返回新的 cap
		PNType Alloc(PNType& idx) {
			if (count) {
				idx = header;
				header = Next(buf[idx]);
				--count;
				return 0;
			}
			else {
				if (len == cap) {
					cap *= 2;
					Reserve();
				}
				idx = len;
				++len;
				return cap;
			}
		}

		void Free(PNType const& idx) {
			buf[idx].~T();
			Next(buf[idx]) = header;
			header = idx;
			++count;
		}

		void Clear() {
			for (int i = 0; i < len; ++i) {
				buf[i].~T();
			}
			len = 0;
			count = 0;
			header = -1;
		}

		void Reserve(PNType const& cap) {
			if (this->cap <= cap) return;
			this->cap = cap;
			Reserve();
		}

		PNType Len() const {
			return len;
		}
		PNType Count() const {
			return len - count;
		}

		T& operator[](PNType const& idx) {
			assert(idx >= 0 && idx < len);
			return buf[idx];
		}
		T const& operator[](PNType const& idx) const {
			assert(idx >= 0 && idx < len);
			return buf[idx];
		}
	protected:
		void Reserve() {
			if constexpr (IsReallocable_v<T>) {
				buf = (T*)realloc((void*)buf, sizeof(T) * cap);
			}
			else {
				T* newBuf = (T*)malloc(sizeof(T) * cap);
				for (int i = 0; i < len; ++i) {
					new(&newBuf[i]) T((T&&)buf[i]);
					buf[i].~T();
				}
				free((void*)buf);
				buf = newBuf;
			}
		}
	};

}
