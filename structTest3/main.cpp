#include "xx_combopool.h"

#include <string>
#include <iostream>
#include <chrono>

// slices
struct A {
	XX_SLICE_BACE_CODE(A);
	std::array<char, 512> dummy;
	std::string name;
};
struct B {
	float x, y;
};
struct C {
	int hp;
};

// items
using ABC = xx::Combo<A, B, C>;
using AB = xx::Combo<A, B>;
using AC = xx::Combo<A, C>;
using BC = xx::Combo<B, C>;

using MyEnv = xx::ComboPool<xx::SlicesContainer<A, B, C>, xx::CombosContainer<ABC, AB, AC, BC>>;
std::vector<MyEnv::Shared<ABC>> abcHolders;

struct Foo {
	std::array<char, 512> dummy;
	std::string name;
	float x, y;
	int hp;
};
std::vector<Foo> foos;
std::vector<std::shared_ptr<Foo>> fooPtrs;

int main() {
	MyEnv env;

	for (size_t i = 0; i < 100000; i++) {
		auto& f = foos.emplace_back();
		f.name = "name_" + std::to_string(i);
		f.x = (float)i;
		f.y = (float)i;
		f.hp = (int)i;

		fooPtrs.emplace_back(std::make_shared<Foo>(f));

		auto&& abc = env.CreateCombo<ABC>();
		abc.RefSlice<A>().name = f.name;
		abc.RefSlice<B>().x = f.x;
		abc.RefSlice<B>().y = f.y;
		abc.RefSlice<C>().hp = f.hp;
		abcHolders.push_back(std::move(abc));
	}

	uint64_t totalHP = 0;
	auto tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < 10000; i++) { for (auto& f : foos) { totalHP += f.hp; } }
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for foos ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < 10000; i++) { for (auto& f : fooPtrs) { totalHP += f->hp; } }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for fooPtrs ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < 10000; i++) { env.ForeachSlices<C>([&](C& o, auto& owner) { totalHP += o.hp; }); }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "env.ForeachSlices<C> ms = " << ms << ", totalHP = " << totalHP << std::endl;

	return 0;
}








//
//#include <chrono>
//
//// slices
//struct A {
//	SliceBaseCode(A);
//	std::array<char, 512> dummy;
//	std::string name;
//};
//struct B {
//	float x, y;
//};
//struct C {
//	int hp;
//};
//
//// items
//using ABC = O<A, B, C>;
//using AB = O<A, B>;
//using AC = O<A, C>;
//using BC = O<B, C>;
//
//using MyEnv = Env<Vectors<A, B, C>, LinkPools<ABC, AB, AC, BC>>;
//
//int main() {
//	MyEnv env;
//	{
//		std::vector<MyEnv::Shared<ABC>> abcHolders;
//		abcHolders.reserve(5);
//		abcHolders.push_back(env.CreateItem<ABC>());
//		abcHolders.push_back(env.CreateItem<ABC>());
//		std::cout << std::endl;
//	}
//	return 0;
//}









//
//// slices
//struct A {
//	SliceBaseCode(A);
//	std::string name;
//};
//struct B {
//	float x, y;
//};
//struct C {
//	int hp;
//};
//
//// items
//using ABC = O<A, B, C>;
//using AB = O<A, B>;
//using AC = O<A, C>;
//using BC = O<B, C>;
//
//using MyEnv = Env<Vectors<A, B, C>, LinkPools<ABC, AB, AC, BC>>;
//
//std::array<char const*, 4> itemTypes{ "ABC", "AB", "AC", "BC" };
//
//int main() {
//	MyEnv env;
//
//	auto&& abc = env.CreateItem<ABC>();
//	abc.RefSlice<A>().name = "asdf";
//	abc.RefSlice<B>().x = 1;
//	abc.RefSlice<B>().y = 2;
//	abc.RefSlice<C>().hp = 3;
//
//	// 出括号后自杀
//	{
//		auto&& bc = env.CreateItem<BC>();
//		bc.RefSlice<B>().x = 1.2f;
//		bc.RefSlice<B>().y = 3.4f;
//		bc.RefSlice<C>().hp = 5;
//	}
//
//	auto&& ac = env.CreateItem<AC>();
//	ac.RefSlice<A>().name = "qwerrrr";
//	ac.RefSlice<C>().hp = 7;
//
//	auto&& ac2 = env.CreateItem<AC>();
//	ac2.RefSlice<A>().name = "e";
//	ac2.RefSlice<C>().hp = 9;
//
//	env.ForeachSlices<A>([](A& o, auto& owner) {
//		std::cout << "owner: " << itemTypes[owner.first] << ", idx = " << owner.second << std::endl;
//		std::cout << "A::name = " << o.name << std::endl;
//		});
//	env.ForeachSlices<B>([](B& o, auto& owner) {
//		std::cout << "owner: " << itemTypes[owner.first] << ", idx = " << owner.second << std::endl;
//		std::cout << "B::x y = " << o.x << ", " << o.y << std::endl;
//		});
//	env.ForeachSlices<C>([](C& o, auto& owner) {
//		std::cout << "owner: " << itemTypes[owner.first] << ", idx = " << owner.second << std::endl;
//		std::cout << "C::hp = " << o.hp << std::endl;
//		});
//
//	return 0;
//}
//













//struct Bar {
//	Bar() = default;
//	Bar(Bar const&) {
//		std::cout << "Bar const&" << std::endl;
//	}
//	Bar& operator=(Bar const&) {
//		std::cout << "=Bar const&" << std::endl;
//		return *this;
//	}
//	Bar(Bar&&) { 
//		std::cout << "Bar&&" << std::endl;
//	}
//	Bar& operator=(Bar&&) { 
//		std::cout << "=Bar&&" << std::endl;
//		return *this;
//	}
//};
//struct Foo { 
//	SliceBaseCode(Foo)
//	Bar bar;
//};
