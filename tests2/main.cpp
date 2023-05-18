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

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("tests2");
}
