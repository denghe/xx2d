// todo: ECS思考: 用组合来替代继承？类似列存储？数据按需求分组聚合？

#include "LinkPool.h"
#include <vector>
#include <tuple>

struct A {
	int a;
};
struct B {
	int b;
};
struct C {
	int c;
};

template<typename T>
struct W : T {
	int prev;
	int next;
	int self;
};

template<typename T>
using E = std::pair<LinkPool<W<T>>, int>;	// second: header

std::tuple<E<A>, E<B>, E<C>> pool;

using Item = std::array<int, 3>;

std::vector<Item> items;


template <class T, class Tuple>
struct Index;

template <class T, class... Types>
struct Index<T, std::tuple<T, Types...>> {
	static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>> {
	static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
};


template<class Tuple, std::size_t N>
struct PoolIniter {
	static void Init(Tuple& in) {
		std::get<N - 1>(in).second = -1;
		PoolIniter<Tuple, N - 1>::Init(in);
	}
};

template<class Tuple>
struct PoolIniter<Tuple, 0> {
	static void Init(Tuple& in) {
		std::get<0>(in).second = -1;
	}
};

template<class Tuple>
void PoolInit(Tuple& in) {
	PoolIniter<Tuple, std::tuple_size<Tuple>::value>::Init(in);
}

struct StaticPoolIniter {
	StaticPoolIniter() {
		PoolInit(pool);
	}
} __StaticPoolIniter;

template<typename T>
struct Component {
	int idx;
	LinkPool<W<T>>& Pool() { return std::get<E<T>>(pool).first; }
	int& Header() { return std::get<E<T>>(pool).second; }
	W<T>& Node() { return Pool()[idx]; }
	T& Data() { return (T&)Node(); }
	operator T& () { return Data(); }
	explicit Component() {
		auto& p = Pool();
		p.Alloc(idx);
		auto& o = Node();
		auto& h = Header();
		o.prev = -1;
		o.next = h;
		if (h != -1) {
			p[h].prev = idx;
		}
		h = idx;
		//o.self = self;
	}
	~Component() {
		auto& p = Pool();
		auto& h = Header();
		auto& o = Node();
		if (o.prev != -1) {
			p[o.prev].next = o.next;
		}
		if (o.next != -1) {
			p[o.next].prev = o.prev;
		}
		if (h == idx) {
			h = o.next;
		}
		p.Free(idx);
	}
};

struct AB : Component<A>, Component<B> {
	AB() {
		// todo: get self's idx
		this->Component<A>::Node().self = 0;
		this->Component<B>::Node().self = 0;
	}
};

int main() {
	//AB ab;
	//ab.Component<A>::i = 1;
	return 0;
}

//template<size_t capA, size_t capB, size_t capC>
//struct Env {
//	Env() : as(capA), bs(capB), cs(capC) {}
//	LinkPool<W<A>> as;
//	int asHeader = -1;
//	LinkPool<W<B>> bs;
//	int bsHeader = -1;
//	LinkPool<W<C>> cs;
//	int csHeader = -1;
//};
//Env<10000, 10000, 10000> env;
//
//struct AB {
//	int ia;
//	A& a() { return env.as[ia]; }
//	operator A&() { return a(); }
//
//	int ib;
//	B& b() { return env.bs[ib]; }
//	operator B& () { return b(); }
//
//	AB() {
//		env.as.Alloc(ia);
//		env.bs.Alloc(ib);
//	}
//	~AB() {
//		env.as.Free(ia);
//		env.bs.Free(ib);
//	}
//};
//struct BC {
//	int ib;
//	B& b() { return env.bs[ib]; }
//	operator B& () { return b(); }
//
//	int ic;
//	C& c() { return env.cs[ic]; }
//	operator C& () { return c(); }
//
//	BC() {
//		env.bs.Alloc(ib);
//		env.cs.Alloc(ic);
//	}
//	~BC() {
//		env.bs.Free(ib);
//		env.cs.Free(ic);
//	}
//};

//struct World {
//	std::vector<AB> abs;
//};
