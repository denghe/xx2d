#include "main.h"


void GameLooper::AfterGLInit() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	tp.Load("res/gemini/gemini.plist");
}

int GameLooper::Update() {
	timePool += xx::engine.delta;
	while (timePool >= fds) {
		timePool -= fds;
		
	}

	fpsViewer.Draw(fontBase);
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("arcade: gemini");
}
