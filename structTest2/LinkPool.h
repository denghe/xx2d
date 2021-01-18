#include <cstring>	// malloc realloc
//#include <cstddef>	// offsetof
#include <type_traits>

// 链表节点池. 分配的是不会变化的 下标. 可指定 next 变量类型和偏移量. 当前 T 仅支持 pod 类型

template<typename T, typename PNType = int, size_t nextOffset = 0>
class LinkPool {
	static_assert(std::is_standard_layout_v<T>);
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
	explicit LinkPool(PNType const& cap) : cap(cap) {
		buf = (T*)malloc(sizeof(T) * cap);
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
				buf = (T*)realloc((void*)buf, sizeof(T) * cap);
			}
			idx = len;
			++len;
			return cap;
		}
	}

	void Free(PNType const& idx) {
		Next(buf[idx]) = header;
		header = idx;
		++count;
	}

	void Clear() {
		len = 0;
		count = 0;
		header = -1;
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
};
