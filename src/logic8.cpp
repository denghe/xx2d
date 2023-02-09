#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Logic8::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic8 Init( node tests )" << std::endl;

	t.Emplace(xx::engine.LoadTexture("res/mouse.pkm"));
	float x = -250;
	for (size_t i = 0; i < 11; i++) {
		auto& [l, s] = n.ss.emplace_back();
		//l.SetText(logic->fnt1, xx::ToString("mouse",i), 16);
		//l.SetPositon({ x + i * 50, -20 });

		s.SetPositionX(x + i * 50);
		s.SetTexture(t);
	}
}

int Logic8::Update() {

	n.radians += 0.001f;
	//n.scale += 0.0001f;
	n.at = xx::AffineTransform::MakePosScaleRadians(n.pos, n.scale, n.radians);

	for (auto& [l, s] : n.ss) {
		s.SetScale(s.scale + 0.0001f);
		s.Draw(n.at);

		l.SetRotate(l.radians - 0.001f);
		l.Draw(n.at);
	}

	return 0;
}
