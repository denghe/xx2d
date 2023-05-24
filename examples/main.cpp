#include "main.h"
#include "s0_main_menu.h"
#ifndef __EMSCRIPTEN__
#include "imgui.h"
#endif

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);

	scene = xx::Make<MainMenu::Scene>();
	scene->Init(this);

#ifndef __EMSCRIPTEN__
	xx::engine.imguiInit = [] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();

		//ImFontConfig cfg;
		//cfg.SizePixels = 26.f;
		//auto&& imfnt = io.Fonts->AddFontDefault(&cfg);
		auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());

		io.Fonts->Build();
		io.FontDefault = imfnt;

		io.IniFilename = nullptr;
	};
#endif
}

int GameLooper::Update() {
	if (xx::engine.Pressed(xx::KbdKeys::Escape)) {
		if (!dynamic_cast<MainMenu::Scene*>(scene.pointer)) {
			DelaySwitchTo<MainMenu::Scene>();
		}
	}

	int r = scene->Update();

	fpsViewer.Draw(fontBase);
	return r;
}




int main() {
#ifndef __EMSCRIPTEN__
	auto g = std::make_unique<GameLooper>();
#else
	auto g = new GameLooper();
#endif
	return g->Run("xx2d's examples");
}
