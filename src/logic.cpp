#include "pch.h"
#include "logic.h"
#include "logic0.h"

void Logic::Init() {
	fnt1 = xx::engine.LoadBMFont("res/font1/basechars.fnt"sv);

	lbInfo.SetPosition(xx::engine.ninePoints[1] + xx::XY{ 10, 10 }).SetAnchor({0, 0});

	lg = xx::Make<Logic0>();
	lg->Init(this);
}

int Logic::Update() {
	if (xx::engine.Pressed(xx::KbdKeys::Escape)) {
		if (!dynamic_cast<Logic0*>(lg.pointer)) {
			DelaySwitchTo<Logic0>();
		}
	}

	int r = lg->Update();

	// display draw call, fps ...
	++fpsCounter;
	fpsTimePool += xx::engine.delta;
	if (fpsTimePool >= 1) {
		fpsTimePool -= 1;
		fps = fpsCounter;
		fpsCounter = 0;
	}
	lbInfo.SetText(fnt1, xx::ToString("fps = ", fps, ", draw call = ", xx::engine.sm.GetDrawCall(), ", quad count = ", xx::engine.sm.GetDrawQuads(), ", line point count = ", xx::engine.sm.GetDrawLines(), std::string_view(extraInfo)))
		.SetPosition({ lbInfo.pos.x + 2, lbInfo.pos.y - 2 })
		.SetColor({ 0, 0, 255, 255 })
		.Draw();
	lbInfo.SetPosition({ lbInfo.pos.x - 2, lbInfo.pos.y + 2 })
		.SetColor({ 255, 0, 0, 255 })
		.Draw();	// shadow
	return r;
}
