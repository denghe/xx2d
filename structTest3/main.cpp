#include <type_traits>
#include <vector>
#include <tuple>
#include <string>
#include <cassert>

// 模拟一下 ECS 的内存分布特性（ 按业务需求分组的 数据块 连续高密存放 )
// 组合优先于继承, 拆分后最好只有数据, 没有函数

/*********************************************************************/
// 基础库代码

// 每个数据分块继承这个 省掉公共代码
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

/*********************************************************************/
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

struct Env;

// 排列组合
using ABC = std::tuple<I<A>, I<B>, I<C>, Env*>;
using AB = std::tuple<I<A>, I<B>, Env*>;
using AC = std::tuple<I<A>, I<C>, Env*>;
using BC = std::tuple<I<B>, I<C>, Env*>;

struct Env {
	std::tuple<
		std::vector<A>, std::vector<B>, std::vector<C>,
		std::vector<ABC>, std::vector<AB>, std::vector<AC>, std::vector<BC>
	> data;
	template<typename T> std::vector<T> const& Data() const { return std::get<std::vector<T>>(data); }
	template<typename T> std::vector<T>& Data() { return std::get<std::vector<T>>(data); }

	template<typename T>
	void Create(int const& self, int& idx) {
		auto& d = Data<T>();
		idx = (int)d.size();
		d.emplace_back().self = self;
	}
	template<typename T>
	void Release(int const& idx, int const& self) {
		auto& d = Data<T>();
		assert(idx >= 0 && idx < d.size());
		d[idx] = std::move(d.back());
		d[idx].self = self;
		d.pop_back();
	}
	template<typename O, typename T>
	O& Ref(T& t) {
		return Data<O>()[std::get<I<O>>(t)];
	}
	template<typename O, typename T>
	O const& Ref(T const& t) const {
		return Data<O>()[std::get<I<O>>(t)];
	}

	template<typename T>
	T& Create() {
		auto& d = Data<ABC>();
		int self = (int)d.size();
		auto& r = d.emplace_back();
		if constexpr (TupleContainsType_v<I<A>, T>) { Create<A>(self, std::get<I<A>>(r)); }
		if constexpr (TupleContainsType_v<I<B>, T>) { Create<B>(self, std::get<I<B>>(r)); }
		if constexpr (TupleContainsType_v<I<C>, T>) { Create<C>(self, std::get<I<C>>(r)); }
		std::get<Env*>(r) = this;
		return r;
	}

	template<typename T>
	void Release(int const& idx) {
		auto& d = Data<ABC>();
		assert(idx >= 0 && idx < (int)d.size());
		auto& r = d[idx];
		if constexpr (TupleContainsType_v<I<A>, T>) { Release<A>(std::get<I<A>>(r), idx); }
		if constexpr (TupleContainsType_v<I<B>, T>) { Release<B>(std::get<I<B>>(r), idx); }
		if constexpr (TupleContainsType_v<I<C>, T>) { Release<C>(std::get<I<C>>(r), idx); }
		r = std::move(d.back());
		d.pop_back();
	}
};

/*********************************************************************/
// 附加简单 dump 功能

#include <iostream>

template<typename T, typename ENABLED = std::enable_if_t<TupleContainsType_v<T, std::tuple<ABC, AB, AC, BC>>>>
void Dump(std::ostream& os, T const& o) {
	auto& env = *std::get<Env*>(o);
	if constexpr (TupleContainsType_v<I<A>, T>) os << env.Ref<A>(o) << std::endl;
	if constexpr (TupleContainsType_v<I<B>, T>) os << env.Ref<B>(o) << std::endl;
	if constexpr (TupleContainsType_v<I<C>, T>) os << env.Ref<C>(o) << std::endl;
}

std::ostream& operator<<(std::ostream& os, A const& o) {
	return os << "A.name = " << o.name;
}
std::ostream& operator<<(std::ostream& os, B const& o) {
	return os << "B.x,y = " << o.x << " " << o.y;
}
std::ostream& operator<<(std::ostream& os, C const& o) {
	return os << "C.hp = " << o.hp;
}
std::ostream& operator<<(std::ostream& os, ABC const& o) { Dump(os, o);	return os; }
std::ostream& operator<<(std::ostream& os, AB const& o) { Dump(os, o);	return os; }
std::ostream& operator<<(std::ostream& os, AC const& o) { Dump(os, o);	return os; }
std::ostream& operator<<(std::ostream& os, BC const& o) { Dump(os, o);	return os; }



/*********************************************************************/
// 测试

int main() {
	Env env;
	auto& abc = env.Create<ABC>();
	env.Ref<A>(abc).name = "asdf";
	env.Ref<B>(abc).x = 1;
	env.Ref<B>(abc).y = 2;
	env.Ref<C>(abc).hp = 3;
	std::cout << abc << std::endl;

	return 0;
}
