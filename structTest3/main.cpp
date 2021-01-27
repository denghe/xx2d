#include "xx_combopool.h"
#include <string>
#include <iostream>
#include <chrono>

// classic struct
struct Foo {
	std::array<char, 512> dummy;
	std::string name;
	float x, y;
	int hp;
};

// slices
struct A {
	XX_MOVEONLY_STRUCT_BASE_CODE(A);
	std::array<char, 512> dummy;
	std::string name;
};
struct B {
	float x, y;
};
struct C {
	int hp;
};

// remix classic & combo. 
struct E : xx::Combo<C> {
	XX_MOVEONLY_STRUCT_BASE_CODE(E);
	std::array<char, 512> dummy;
	std::string name;
	float x, y;
};

// pure combos
using ABC = xx::Combo<A, B, C>;
using AB = xx::Combo<A, B>;
using AC = xx::Combo<A, C>;
using BC = xx::Combo<B, C>;

using CP = xx::ComboPool<xx::SlicesContainer<A, B, C>, xx::CombosContainer<ABC, AB, AC, BC, E>>;


// Shared / Weak test
void Test1() {
	CP cp;
	std::vector<CP::Shared<ABC>> abcShareds;
	CP::Weak<ABC> w = abcShareds.emplace_back(cp.CreateCombo<ABC>());
	std::cout << (bool)w << std::endl;
	abcShareds.clear();
	std::cout << (bool)w << std::endl;
}

// performance test
void Test2() {
	std::vector<Foo> foos;
	std::vector<std::shared_ptr<Foo>> fooPtrs;
	CP cp;
	std::vector<CP::Shared<ABC>> abcHolders;

	int num = 1000000;
	int times = 1000;

	foos.reserve(num);
	fooPtrs.reserve(num);
	cp.Reserve(num);
	abcHolders.reserve(num);

	auto tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < num; i++) {
		auto& f = foos.emplace_back();
		f.name = "name_" + std::to_string(i);
		f.x = (float)i;
		f.y = (float)i;
		f.hp = (int)i;
	}
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "insert foos count = " << num << " ms = " << ms<< std::endl;

	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < num; i++) {
		auto& f = fooPtrs.emplace_back(std::make_shared<Foo>());
		f->name = "name_" + std::to_string(i);
		f->x = (float)i;
		f->y = (float)i;
		f->hp = (int)i;
	}
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "insert fooPtrs count = " << num << " ms = " << ms << std::endl;

	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < num; i++) {
		auto&& abc = abcHolders.emplace_back(cp.CreateCombo<ABC>());
		abc.Visit<A>().name = "name_" + std::to_string(i);
		abc.Visit<B>().x = (float)i;
		abc.Visit<B>().y = (float)i;
		abc.Visit<C>().hp = (int)i;
	}
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "insert abcHolders count = " << num << " ms = " << ms << std::endl;

	uint64_t totalHP = 0;
	for (size_t i = 0; i < times; i++) { for (auto& f : foos) { totalHP += f.hp; } }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for foos " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < times; i++) { for (auto& f : fooPtrs) { totalHP += f->hp; } }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for fooPtrs " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < times; i++) { for (auto& f : abcHolders) { totalHP += f.Visit<C>().hp; } }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for abcHolders " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < times; i++) { cp.ForeachSlices<C>([&](C& o, auto& owner) { totalHP += o.hp; }); }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "call cp.ForeachSlices<C> " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;
}


// performance test2
void Test3() {
	CP cp;
	std::vector<CP::Shared<E>> eHolders;

	int num = 1000000;
	int times = 1000;

	cp.Reserve(num);
	eHolders.reserve(num);

	auto tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < num; i++) {
		auto&& e = eHolders.emplace_back(cp.CreateCombo<E>());
		e->name = "name_" + std::to_string(i);
		e->x = (float)i;
		e->y = (float)i;
		e.Visit<C>().hp = (int)i;
	}
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "insert eHolders count = " << num << " ms = " << ms << std::endl;

	uint64_t totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < times; i++) { for (auto& f : eHolders) { totalHP += f.Visit<C>().hp; } }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "for eHolders " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;

	totalHP = 0;
	tp = std::chrono::system_clock::now();
	for (size_t i = 0; i < times; i++) { cp.ForeachSlices<C>([&](C& o, auto& owner) { totalHP += o.hp; }); }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "call cp.ForeachSlices<C> " << times << " times ms = " << ms << ", totalHP = " << totalHP << std::endl;
}


int main() {
	Test1();
	Test2();
	Test3();
	return 0;
}

