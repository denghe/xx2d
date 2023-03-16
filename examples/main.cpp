#include "main.h"
#include "s0_main_menu.h"

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

	scene = xx::Make<MainMenu::Scene>();
	scene->Init(this);

	xx::engine.imguiInit = [] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();

		//ImFontConfig cfg;
		//cfg.SizePixels = 26.f;
		//auto&& imfnt = io.Fonts->AddFontDefault(&cfg);
		auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());

		io.Fonts->Build();
		io.FontDefault = imfnt;
	};
}

int GameLooper::Update() {
	if (xx::engine.Pressed(xx::KbdKeys::Escape)) {
		if (!dynamic_cast<MainMenu::Scene*>(scene.pointer)) {
			DelaySwitchTo<MainMenu::Scene>();
		}
	}

	int r = scene->Update();

	fpsViewer.Update();
	return r;
}

int main() {
	return GameLooper{}.Run("xx2d's examples");
}
