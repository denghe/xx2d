#include <type_traits>
#include <vector>
#include <tuple>
#include <array>
#include <string>
#include <cassert>

#include <iostream>

#include "LinkPool.h"


// 模拟一下 ECS 的内存分布特性（ 按业务需求分组的 数据块 连续高密存放 )
// 组合优先于继承, 拆分后最好只有数据, 没有函数

/*********************************************************************/
// 基础库代码

// 每个 拆分后的数据块 继承这个 省掉公共代码	// todo: 需要验证这样写之后 std::move 是否正确工作

#define SliceDefaultCodes(T)		\
T() = default;						\
T(T const&) = delete;				\
T& operator=(T const&) = delete;	\
T(T&&) = default;					\
T& operator=(T&&) = default;		\
int owner = -1;


// 组合后的类的基础数据 int 包装, 便于 tuple 类型查找定位

template<typename T>
struct I {
	int i;
	operator int& () { return i; }
	operator int const& () const { return i; }
};


// 组合后的类之本体. 使用 using 来定义类型

template<typename...TS>
struct O {
	O() = default;
	O(O const&) = delete;
	O& operator=(O const&) = delete;
	O(O&&) = default;
	O& operator=(O&&) = default;
	int idx, prev, next;
	std::tuple<I<TS>...> data;
};


// 判断 tuple 里是否存在某种数据类型

template <typename T, typename Tuple>
struct HasType;

template <typename T>
struct HasType<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct HasType<T, std::tuple<U, Ts...>> : HasType<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct HasType<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename T, typename Tuple>
using TupleContainsType = typename HasType<T, Tuple>::type;

template <typename T, typename Tuple>
constexpr bool TupleContainsType_v = TupleContainsType<T, Tuple>::value;


// 计算某类型在 tuple 里是第几个

template <class T, class Tuple>
struct TupleTypeIndex;

template <class T, class...TS>
struct TupleTypeIndex<T, std::tuple<T, TS...>> {
	static const size_t value = 0;
};

template <class T, class U, class... TS>
struct TupleTypeIndex<T, std::tuple<U, TS...>> {
	static const size_t value = 1 + Index<T, std::tuple<TS...>>::value;
};

template <typename T, typename Tuple>
constexpr size_t TupleTypeIndex_v = TupleTypeIndex<T, Tuple>::value;


// 类型列表转为 tuple vector 列表

template<typename...TS>
struct Vectors {
	using Types = std::tuple<TS...>;
	std::tuple<std::vector<TS>...> vectors;
	template<typename T> std::vector<T> const& Get() const { return std::get<std::vector<T>>(vectors); }
	template<typename T> std::vector<T>& Get() { return std::get<std::vector<T>>(vectors); }
};

template<typename...TS>
struct LinkPools {
	using Types = std::tuple<TS...>;
	std::tuple<LinkPool<TS>...> linkpools;
	template<typename T> LinkPool<T> const& Get() const { return std::get<LinkPool<T>>(linkpools); }
	template<typename T> LinkPool<T>& Get() { return std::get<LinkPool<T>>(linkpools); }
};



template<typename Slices, typename Items>
struct Env {
	// 拆分后的数据块 容器
	Slices slices;
	// Items 容器
	Items items;
	// 针对每种类型的 链表头
	std::array<int, std::tuple_size_v<typename Items::Types>> headers;

	template<typename T>
	void CreateData(int const& owner, int& idx) {
		auto& d = slices.Get<T>();
		idx = (int)d.size();
		d.emplace_back().owner = owner;
	}
	template<typename T>
	void ReleaseData(int const& owner, int const& idx) {
		auto& d = slices.Get<T>();
		assert(idx >= 0 && idx < d.size());
		d[idx] = std::move(d.back());
		d[idx].owner = owner;
		d.pop_back();
	}

	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t<i == std::tuple_size_v<Tp>> TryCreate(int const& owner, T& r) {}
	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t < i < std::tuple_size_v<Tp>> TryCreate(int const& owner, T& r) {
		using O = std::decay_t<decltype(std::get<i>(std::declval<Tp>()))>;
		if constexpr (TupleContainsType_v<I<O>, T>) {
			CreateData<O>(owner, std::get<I<O>>(r));
		}
		TryCreate<i + 1, Tp, T>(owner, r);
	}

	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t<i == std::tuple_size_v<Tp>> TryRelease(int const& owner, T const& r) {}
	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t < i < std::tuple_size_v<Tp>> TryRelease(int const& owner, T const& r) {
		using O = std::decay_t<decltype(std::get<i>(std::declval<Tp>()))>;
		if constexpr (TupleContainsType_v<I<O>, T>) {
			ReleaseData<O>(owner, std::get<I<O>>(r));
		}
		TryRelease<i + 1, Tp, T>(owner, r);
	}


	template<typename T>
	int& Header() {
		return headers[TupleTypeIndex_v<T, typename Items::Types>];
	}
	// todo: store  header for foreach

	template<typename T>
	T& CreateItem() {
		auto& d = items.Get<T>();
		int idx;
		d.Alloc(idx);
		auto& r = d[idx];
		TryCreate<0, typename decltype(slices)::Types>(idx, r.data);
		r.idx = idx;
		return r;
	}

	template<typename T>
	void ReleaseItem(int const& idx) {
		auto& d = items.Get<T>();
		auto& r = d[idx];
		TryRelease<0, typename decltype(slices)::Types>(idx, r.data);
		d.Free(idx);
	}

	//template<typename Item>
	//Item& RefItem(int const& idx) {
	//	return items.Get<Item>()[idx];
	//}
	//template<typename Item>
	//Item const& RefItem(int const& idx) const {
	//	return items.Get<Item>()[idx];
	//}

	template<typename Slice, typename Item>
	Slice& RefSlice(Item& o) {
		auto&& idx = std::get<I<Slice>>(o.data);
		return slices.Get<Slice>()[idx];
	}
	template<typename Slice, typename Item>
	Slice const& RefSlice(Item const& o) const {
		auto&& idx = std::get<I<Slice>>(o.data);
		return slices.Get<Slice>()[idx];
	}
};


/*********************************************************************/
// tests

// slices
struct A { SliceDefaultCodes(A)
	std::string name;
};
struct B { SliceDefaultCodes(B)
	float x = 0, y = 0;
};
struct C { SliceDefaultCodes(C)
	int hp = 0;
};

// items
using ABC = O<A, B, C>;
using AB = O<A, B>;
using AC = O<A, C>;
using BC = O<B, C>;

using MyEnv = Env<Vectors<A, B, C>, LinkPools<ABC, AB, AC, BC>>;

// todo: 将每一种类型分别封装成智能指针?

template<typename T>
struct Base_r {
	MyEnv& _env;
	T& _item;
	Base_r(MyEnv& env, T& item) : _env(env), _item(item) {}
};

#define A_r \
std::string& name() { return _env.RefSlice<A>(_item).name; }				\
std::string const& name() const { return _env.RefSlice<A>(_item).name; }

#define B_r \
float& x() { return _env.RefSlice<B>(_item).x; }							\
float const& x() const { return _env.RefSlice<B>(_item).x; }				\
float& y() { return _env.RefSlice<B>(_item).y; }							\
float const& y() const { return _env.RefSlice<B>(_item).y; }

#define C_r \
int& hp() { return _env.RefSlice<C>(_item).hp; }							\
int const& hp() const { return _env.RefSlice<C>(_item).hp; }

struct ABC_r : Base_r<ABC> {
	using Base_r<ABC>::Base_r;
	A_r
	B_r
	C_r
};

struct Bar {
	Bar() = default;
	Bar(Bar const&) {
		std::cout << "Bar const&" << std::endl;
	}
	Bar& operator=(Bar const&) {
		std::cout << "=Bar const&" << std::endl;
		return *this;
	}
	Bar(Bar&&) { 
		std::cout << "Bar&&" << std::endl;
	}
	Bar& operator=(Bar&&) { 
		std::cout << "=Bar&&" << std::endl;
		return *this;
	}
};
struct Foo { 
	SliceDefaultCodes(Foo)
	Bar bar;
};

int main() {
	MyEnv env;
	auto&& abc = env.CreateItem<ABC>();
	ABC_r o(env, abc);
	o.name() = "asdf";
	o.x() = 1;
	o.y() = 2;
	o.hp() = 3;
	
	std::cout << "name = " << o.name() << " "
		<< "x = " << o.x() << " "
		<< "y = " << o.y() << " "
		<< "hp = " << o.hp() << std::endl;


	Foo f;
	auto&& f2 = std::move(f);
	Bar b1, b2;
	b2 = std::move(b1);
	return 0;
}
