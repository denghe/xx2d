#include "main.h"
#include "s0_main_menu.h"
#include "imgui.h"

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







#include <xx_listlink.h>

int main() {

	int counter = 0, n = 1, m = 5000000;
	auto secs = xx::NowEpochSeconds();

	for (size_t i = 0; i < n; i++) {

		xx::ListLink<int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			new (ll.Add()) int(2);
		}

		int prev = -1, next{};
		for (auto idx = ll.head; idx != -1;) {
			if (ll[idx] == 2 || ll[idx] == 4) {
				next = ll.Remove(idx, prev);
			} else {
				next = ll.Next(idx);
			}
			prev = idx;
			idx = next;
		}
		new (ll.Add()) int(2);
		new (ll.Add()) int(4);

		for (auto idx = ll.head; idx != -1; idx = ll.Next(idx)) {
			counter += ll[idx];
		}
	}

	xx::CoutN("ListLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::map<int, int> ll;
		int autoInc{};
		for (size_t j = 1; j <= m; j++) {
			ll[++autoInc] = j;
		}

		for(auto it = ll.begin(); it != ll.end();) {
			if (it->second == 2 || it->second == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll[++autoInc] = 2;
		ll[++autoInc] = 4;

		for (auto&& kv : ll) {
			counter += kv.second;
		}
	}

	xx::CoutN("map counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	return 0;

	//return GameLooper{}.Run("xx2d's examples");
}
