#include "xx2d_pch.h"
#include "game_looper.h"
#include "s0_main_menu.h"

void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);

	lbInfo.SetAnchor({0, 0});

	scene = xx::Make<MainMenu::Scene>();
	scene->Init(this);
}

int GameLooper::Update() {
	if (xx::engine.Pressed(xx::KbdKeys::Escape)) {
		if (!dynamic_cast<MainMenu::Scene*>(scene.pointer)) {
			DelaySwitchTo<MainMenu::Scene>();
		}
	}

	int r = scene->Update();

	// calc frame time
	++fpsCounter;
	fpsTimePool += xx::engine.delta;
	if (fpsTimePool >= 1) {
		fpsTimePool -= 1;
		fps = fpsCounter;
		fpsCounter = 0;
	}

	// display draw call, fps ...
	auto& sm = xx::engine.sm;
	sm.End();

	auto xy = xx::engine.ninePoints[1] + xx::XY{ 10, 10 };
	auto s = xx::ToStringFormat("fps = {0}, draw call = {1}, vert count = {2}, line point count = {3}; {4}"
		, fps, sm.drawCall, sm.drawVerts, sm.drawLinePoints, std::string_view(extraInfo));
	lbInfo.SetText(fontBase, s, 32.f, xx::engine.w - 40)
		.SetPosition(xy.MakeAdd(2, -2))
		.SetColor({ 0, 0, 255, 255 })
		.Draw()		// shadow
		.SetPosition(xy)
		.SetColor({ 255, 0, 0, 255 })
		.Draw();	// body
	return r;
}
