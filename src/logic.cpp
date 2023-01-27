#include "pch.h"
#include "logic.h"

void Logic::Init() {
	// for display drawcall
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});

	lg.Init(this);
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

	int r = lg.Update();

	// display draw call
	lbCount.SetText(fnt1, xx::ToString("draw call = ", sm.GetDrawCall(), ", quad count = ", sm.GetDrawQuads(), ", line point count = ", sm.GetDrawLines()));
	lbCount.Commit();
	lbCount.Draw(this);
	return r;
}
