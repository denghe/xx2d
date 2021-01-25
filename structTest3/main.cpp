#include <type_traits>
#include <vector>
#include <tuple>
#include <string>
#include <cassert>

#include <iostream>


// 模拟一下 ECS 的内存分布特性（ 按业务需求分组的 数据块 连续高密存放 )
// 组合优先于继承, 拆分后最好只有数据, 没有函数

/*********************************************************************/
// 基础库代码

// 每个 拆分后的数据块 继承这个 省掉公共代码
template<typename T>
struct E {
	E() = default;
	E(E const&) = delete;
	E& operator=(E const&) = delete;
	E(E&&) = default;
	E& operator=(E&&) = default;
	int self = -1;
};

// 组合后的类的基础数据 int 包装, 便于 tuple 类型查找定位
template<typename T>
struct I {
	int i;
	operator int& () { return i; }
	operator int const& () const { return i; }
};

// 通用工具函数 判断 tuple 里是否存在某种数据类型

template <typename T, typename Tuple> struct ContainsType;
// c++11
//template <typename T> struct ContainsType<T, std::tuple<>> : std::false_type {};
//template <typename T, typename U, typename... Ts> struct ContainsType<T, std::tuple<U, Ts...>> : ContainsType<T, std::tuple<Ts...>> {};
//template <typename T, typename... Ts> struct ContainsType<T, std::tuple<T, Ts...>> : std::true_type {};
// c++17
template <typename T, typename... Us> struct ContainsType<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

template <typename T, typename Tuple>
constexpr bool TupleContainsType_v = ContainsType<T, Tuple>::type::value;

// 类型列表转为 tuple vector 列表
template<typename...TS>
struct Vectors {
	using Types = std::tuple<TS...>;
	std::tuple<std::vector<TS>...> data;
	template<typename T> std::vector<T> const& Get() const { return std::get<std::vector<T>>(data); }
	template<typename T> std::vector<T>& Get() { return std::get<std::vector<T>>(data); }
};

// todo: Items 需要放置到 LinkPool 确保返回的 idx 不变 并附带版本号, 以便实现智能指针?

// todo: 在 tuple 中找类型 得到下标的函数

// todo: Env 拆分成 2 部分？ Data, Item ?

// 总上下文
template<typename Data, typename Items>
struct Env {
	Data data;

	template<typename T>
	void CreateData(int const& self, int& idx) {
		auto& d = data.Get<T>();
		idx = (int)d.size();
		d.emplace_back().self = self;
	}
	template<typename T>
	void ReleaseData(int const& self, int const& idx) {
		auto& d = data.Get<T>();
		assert(idx >= 0 && idx < d.size());
		d[idx] = std::move(d.back());
		d[idx].self = self;
		d.pop_back();
	}

	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t<i == std::tuple_size_v<Tp>> TryCreate(int const& self, T& r) {}
	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t<i < std::tuple_size_v<Tp>> TryCreate(int const& self, T& r) {
		using O = std::decay_t<decltype(std::get<i>(std::declval<Tp>()))>;
		if constexpr (TupleContainsType_v<I<O>, T>) {
			CreateData<O>(self, std::get<I<O>>(r));
		}
		TryCreate<i + 1, Tp, T>(self, r);
	}

	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t<i == std::tuple_size_v<Tp>> TryRelease(int const& self, T const& r) {}
	template<size_t i = 0, typename Tp, typename T>	std::enable_if_t < i < std::tuple_size_v<Tp>> TryRelease(int const& self, T const& r) {
		using O = std::decay_t<decltype(std::get<i>(std::declval<Tp>()))>;
		if constexpr (TupleContainsType_v<I<O>, T>) {
			ReleaseData<O>(self, std::get<I<O>>(r));
		}
		TryRelease<i + 1, Tp, T>(self, r);
	}


	/*********************************************************************/
	Items items;
	// todo: 链表头 for items. 下标和类型位置匹配

	template<typename T>
	int CreateItem() {
		auto& d = items.Get<T>();
		int self = (int)d.size();
		auto& r = d.emplace_back();
		TryCreate<0, typename decltype(data)::Types>(self, r);
		return self;
	}

	template<typename T>
	void ReleaseItem(int const& idx) {
		auto& d = items.Get<T>();
		assert(idx >= 0 && idx < (int)d.size());
		auto& r = d[idx];
		TryRelease<0, typename decltype(data)::Types>(idx, r);
		r = std::move(d.back());
		std::get<int>(r) = idx;
		d.pop_back();
	}

	template<typename Item>
	Item& Ref(int const& idx) {
		return items.Get<Item>()[idx];
	}
	template<typename Item>
	Item const& Ref(int const& idx) const {
		return items.Get<Item>()[idx];
	}

	template<typename Item, typename Data>
	Data& Ref(int const& idx) {
		return data.Get<Data>()[std::get<I<Data>>(Ref<Item>(idx))];
	}
	template<typename Item, typename Data>
	Data const& Ref(int const& idx) const {
		return data.Get<Data>()[std::get<I<Data>>(Ref<Item>(idx))];
	}

	// todo: Ref<T& 版?
};


/*********************************************************************/
// 测试
// 模拟生成物

// 拆分后的数据块
struct A : E<A> {
	std::string name;
};
struct B : E<B> {
	float x = 0, y = 0;
};
struct C : E<C> {
	int hp = 0;
};

// todo: 下面的 tuple 变为类成员? 为类附加便于使用的取值函数

// 排列组合( 具体业务类 )
using ABC = std::tuple<I<A>, I<B>, I<C>>;
using AB = std::tuple<I<A>, I<B>>;
using AC = std::tuple<I<A>, I<C>>;
using BC = std::tuple<I<B>, I<C>>;

using ABCEnv = Env<Vectors<A, B, C>, Vectors<ABC, AB, AC, BC>>;

// 附加简单 dump 功能
//
//template<typename T, typename ENABLED = std::enable_if_t<TupleContainsType_v<T, std::tuple<ABC, AB, AC, BC>>>>
//void Dump(std::ostream& os, T const& o) {
//	auto& env = *std::get<Env*>(o);
//	if constexpr (TupleContainsType_v<I<A>, T>) os << env.Ref<A>(o) << std::endl;
//	if constexpr (TupleContainsType_v<I<B>, T>) os << env.Ref<B>(o) << std::endl;
//	if constexpr (TupleContainsType_v<I<C>, T>) os << env.Ref<C>(o) << std::endl;
//}
//
//std::ostream& operator<<(std::ostream& os, A const& o) {
//	return os << "A.name = " << o.name;
//}
//std::ostream& operator<<(std::ostream& os, B const& o) {
//	return os << "B.x,y = " << o.x << " " << o.y;
//}
//std::ostream& operator<<(std::ostream& os, C const& o) {
//	return os << "C.hp = " << o.hp;
//}
//std::ostream& operator<<(std::ostream& os, ABC const& o) { Dump(os, o);	return os; }
//std::ostream& operator<<(std::ostream& os, AB const& o) { Dump(os, o);	return os; }
//std::ostream& operator<<(std::ostream& os, AC const& o) { Dump(os, o);	return os; }
//std::ostream& operator<<(std::ostream& os, BC const& o) { Dump(os, o);	return os; }
//// 模拟一个传统类, 做性能对比
//struct Foo : E<Foo> {
//	std::string name;
//	float x = 0, y = 0;
//	int hp = 0;
//};
//std::vector<Foo> foos;
//

int main() {
	ABCEnv env;

	int idx = env.CreateItem<ABC>();
	{
		auto& a = env.Ref<ABC, A>(idx);
		a.name = "asdf";
	}
	{
		auto& b = env.Ref<ABC, B>(idx);
		b.x = 1;
		b.y = 2;
	}
	{
		auto& c = env.Ref<ABC, C>(idx);
		c.hp = 3;
	}
	//std::cout << abc << std::endl;

	return 0;
}
