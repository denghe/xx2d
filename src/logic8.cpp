#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Logic8::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic8 Init( node tests )" << std::endl;

	t.Emplace(xx::engine.LoadTexture("res/mouse.pkm"));
	float x = -250;
	for (size_t i = 0; i < 11; i++) {
		auto& s = n.ss.emplace_back();
		s.SetPositionX(x + i * 50);
		s.SetTexture(t);
		s.Commit();
	}
}

int Logic8::Update() {

	n.radians += 0.001f;
	n.transform = xx::AffineTransform::Make(n.pos, {}, n.scale, n.radians);
	for (auto& s : n.ss) {
		s.Draw(n.transform);
	}

	return 0;
}
