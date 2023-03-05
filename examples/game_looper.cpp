#include "xx2d_pch.h"
#include "game_looper.h"
#include "s0_main_menu.h"

void GameLooper::Init() {
	fnt1 = xx::engine.LoadBMFont("res/font1/basechars.fnt"sv);

	lbInfo.SetPosition(xx::engine.ninePoints[1] + xx::XY{ 10, 10 }).SetAnchor({0, 0});

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

	// display draw call, fps ...
	++fpsCounter;
	fpsTimePool += xx::engine.delta;
	if (fpsTimePool >= 1) {
		fpsTimePool -= 1;
		fps = fpsCounter;
		fpsCounter = 0;
	}
	lbInfo.SetText(fnt1, xx::ToString("fps = ", fps, ", draw call = ", xx::engine.sm.GetDrawCall(), ", vert count = ", xx::engine.sm.GetDrawVerts(), ", line point count = ", xx::engine.sm.GetDrawLines(), std::string_view(extraInfo)))
		.SetPosition({ lbInfo.pos.x + 2, lbInfo.pos.y - 2 })
		.SetColor({ 0, 0, 255, 255 })
		.Draw();
	lbInfo.SetPosition({ lbInfo.pos.x - 2, lbInfo.pos.y + 2 })
		.SetColor({ 255, 0, 0, 255 })
		.Draw();	// shadow
	return r;
}
