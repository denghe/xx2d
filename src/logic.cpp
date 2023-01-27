#include "pch.h"
#include "logic.h"

void Logic::Init() {
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);

	lbInfo.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbInfo.SetAnchor({0, 0});
	lbInfo.SetColor({ 255, 255, 0, 255 });

	nowSecs = xx::NowSteadyEpochSeconds();

	lg = xx::Make<Logic0>();
	lg->Init(this);
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;
	delta = xx::NowSteadyEpochSeconds(nowSecs);

	int r = lg->Update();

	// display draw call, fps ...
	++fpsCounter;
	fpsTimePool += delta;
	if (fpsTimePool >= 1) {
		fpsTimePool -= 1;
		fps = fpsCounter;
		fpsCounter = 0;
	}
	lbInfo.SetText(fnt1, xx::ToString("fps = ", fps, ", draw call = ", sm.GetDrawCall(), ", quad count = ", sm.GetDrawQuads(), ", line point count = ", sm.GetDrawLines()));
	lbInfo.Commit();
	lbInfo.Draw(this);
	return r;
}
