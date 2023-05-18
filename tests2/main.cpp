#include "main.h"

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fontBase);

	monsters.EmplaceShared(*this);
}

int GameLooper::Update() {
	timePool += xx::engine.delta;
	while (timePool >= fds) {
		timePool -= fds;
		
		for (auto& m : monsters) {
			m->coro();
		}
	}

	fpsViewer.Update();
	return 0;
}



struct A {
	int64_t typeId = 0;
};

struct B {
	int64_t typeId = 1;
	bool b1{true};
};

struct C1 : B {
	bool b2{};
};

struct C2 {
	int64_t typeId = 2;
	bool b1{ true };
	bool b2{};
};

struct Foo {
	int typeId = 1;
	~Foo() {
		std::cout << "~Foo " << typeId << std::endl;
	}
};
struct Bar {
	int typeId = 2;
	~Bar() {
		std::cout << "~Bar " << typeId << std::endl;
	}
};

int main() {
	xx::Shared<A> c1, c2;
	c1.Emplace<C1>();
	c2.Emplace<C2>();

	std::cout << c1->typeId << std::endl;
	std::cout << c2->typeId << std::endl;
	return 0;


	auto g = std::make_unique<GameLooper>();
	return g->Run("tests2");
}
