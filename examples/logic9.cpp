#include "xx2d_pch.h"
#include "game_looper.h"
#include "logic9.h"

// https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors

void Mouse::Init(Logic9* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color) {
	body.SetTexture(owner->tex)
		.SetPosition(pos)
		.SetScale(scale)
		.SetRotate(-radians + M_PI / 2)
		.SetColor(color);
	baseInc = { std::sin(body.radians), std::cos(body.radians) };
	baseInc *= 2;
}

int Mouse::Update() {
	body.AddPosition(baseInc);
	if (body.pos.x * body.pos.x > (1800 / 2) * (1800 / 2)
		|| body.pos.y * body.pos.y > (1000 / 2) * (1000 / 2)) return 1;
	return 0;
}

void Mouse::Draw() {
	auto c = body.color;
	body.AddPosition({ 3,3 }).SetColor({ 255,127,127,127 }).Draw();
	body.AddPosition({ -3,-3 }).SetColor(c).Draw();
}

void Logic9::Init(GameLooper* looper) {
	this->looper = looper;
	std::cout << "Logic9 Init( sprite tests )" << std::endl;

	tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
}

int Logic9::Update() {

	timePool += xx::engine.delta;
	while (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		for (size_t i = 0; i < 100; i++) {
			radians += 0.005;
			ms.emplace_back().Emplace()->Init(this, {}, radians, 1);
		}

		for (auto i = (ptrdiff_t)ms.size() - 1; i >= 0; --i) {
			auto& m = ms[i];
			if (m->Update()) {
				m = ms.back();
				ms.pop_back();
			}
		}
	}

	for (auto& m : ms) {
		m->Draw();
	}

	return 0;
}
