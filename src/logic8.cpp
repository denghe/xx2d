#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Mice::Init(Mices* owner, xx::XY const& pos_, std::string&& name_) {
	pos = pos_;
	at = this->at.MakePosScaleRadians(pos, scale, radians);

	name.SetText(owner->logic8->logic->fnt1, name_, 16);
	name.SetPositionY(-20);

	body.SetTexture(owner->logic8->t);
}

void Mices::Init(Logic8* logic8) {
	this->logic8 = logic8;
	float x = -250;
	for (size_t i = 0; i < 11; i++) {
		ms.emplace_back().Init(this, { x + i * 50, 0 }, xx::ToString("mouse", i));
	}
}

void Mices::Draw() {
	radians += 0.001f;
	scale += 0.0001f;
	//auto at = xx::AffineTransform::MakePosScaleRadians(pos, scale, radians);
	auto at = xx::AffineTransform::MakeIdentity().Rotate(radians);
	for (auto& m : ms) {
		auto pat = at.MakeConcat(m.at);

		m.body.SetParentAffineTransform(&pat);
		m.body.Draw();

		//m.name.SetRotate(m.name.radians - 0.001f);
		m.name.Draw(pat);
	}
}

void Logic8::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic8 Init( node tests )" << std::endl;

	t.Emplace(xx::engine.LoadTexture("res/mouse.pkm"));

	mices.Init(this);
}

int Logic8::Update() {

	mices.Draw();

	return 0;
}
 