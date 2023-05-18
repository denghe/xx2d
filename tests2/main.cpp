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
	xx::Shared<int> c1, c2;
	c1.Emplace<Foo>();
	c2.Emplace<Bar>();
	std::cout << *c1 << std::endl;
	std::cout << *c2 << std::endl;
	return 0;


	auto g = std::make_unique<GameLooper>();
	return g->Run("tests2");
}
