#pragma once
#include "pch.h"
#include "logic_base.h"

struct Mouse2 {
	xx::Quad body;
	xx::XY baseInc{};
	void Init(xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color = { 255,255,255,255 });
	int Update();
	void Draw();
};

struct Logic10 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	//std::vector<xx::Quad> qs;
	//xx::Rnd rnd;

	std::vector<xx::Shared<Mouse2>> ms;
	float timePool{}, radians{};
};
