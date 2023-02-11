#include "pch.h"
#include "logic.h"
#include "logic11.h"

void Logic11::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic11 Init( quad anchor tests )" << std::endl;

	tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
	q1.SetTexture(tex);
	q1.scale = { 8,8 };
	q1.anchor = { 0.5,0 };

	q2.SetTexture(tex);
	q2.anchor = { 0.5,0 };
	q2.color = {255,0,0,255};
}

int Logic11::Update() {

	q1.Draw();
	q2.Draw();

	return 0;
}
