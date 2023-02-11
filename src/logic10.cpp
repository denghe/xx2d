#include "pch.h"
#include "logic.h"
#include "logic10.h"

void Mouse2::Init(xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color) {
	body.SetTexture(xx::engine.LoadTextureFromCache("res/mouse.pkm"));
	body.pos = pos;
	body.scale = scale;
	body.radians = -radians + M_PI / 2;
	body.color = color;
	baseInc = { std::sin(body.radians), std::cos(body.radians) };
	baseInc *= 2;
}

int Mouse2::Update() {
	body.pos += baseInc;
	if (body.pos.x * body.pos.x > (1800 / 2) * (1800 / 2)
		|| body.pos.y * body.pos.y > (1000 / 2) * (1000 / 2)) return 1;
	return 0;
}

void Mouse2::Draw() {
	auto c = body.color;
	body.pos += xx::XY{3, 3};
	body.color = { 255,127,127,127 };
	body.Draw();
	body.pos -= xx::XY{ 3, 3 };
	body.color = c;
	body.Draw();
}


void Logic10::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic10 Init( quad instance tests )" << std::endl;

	//auto tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
	//qs.resize(100000);
	//for (auto& q : qs) {
	//	q.SetTexture( tex );
	//	q.pos = { (float)rnd.Next(- xx::engine.w/2, xx::engine.w / 2), (float)rnd.Next(-xx::engine.h / 2, xx::engine.h / 2) };
	//	q.scale = 2;
	//	q.radians = 0;
	//	q.color = {255,255,255,255};
	//}
}

int Logic10::Update() {
	//for (auto& q : qs) {
	//	q.radians += 0.01;
	//	q.Draw();
	//}

	timePool += xx::engine.delta;
	while (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		for (size_t i = 0; i < 100; i++) {
			radians += 0.005;
			ms.emplace_back().Emplace()->Init({}, radians, 1);
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
