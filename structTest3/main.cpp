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

// combos
using ABC = xx::Combo<A, B, C>;
using AB = xx::Combo<A, B>;
using AC = xx::Combo<A, C>;
using BC = xx::Combo<B, C>;

using CP = xx::ComboPool<xx::SlicesContainer<A, B, C>, xx::CombosContainer<ABC, AB, AC, BC>>;
std::vector<CP::Shared<ABC>> abcHolders;

struct Foo {
	std::array<char, 512> dummy;
	std::string name;
	float x, y;
	int hp;
};
std::vector<Foo> foos;
std::vector<std::shared_ptr<Foo>> fooPtrs;

int main() {
	CP cp;

	for (size_t i = 0; i < 100000; i++) {
		auto& f = foos.emplace_back();
		f.name = "name_" + std::to_string(i);
		f.x = (float)i;
		f.y = (float)i;
		f.hp = (int)i;

		fooPtrs.emplace_back(std::make_shared<Foo>(f));

		auto&& abc = cp.CreateCombo<ABC>();
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
	for (size_t i = 0; i < 10000; i++) { cp.ForeachSlices<C>([&](C& o, auto& owner) { totalHP += o.hp; }); }
	ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count();
	std::cout << "cp.ForeachSlices<C> ms = " << ms << ", totalHP = " << totalHP << std::endl;

	return 0;
}
