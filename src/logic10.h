#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic10;
struct Mouse2 {
	xx::Quad body;
	xx::XY baseInc{};
	void Init(Logic10* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color = { 255,255,255,255 });
	int Update();
	void Draw();
};

struct Logic10 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	//std::vector<xx::Quad> qs;
	//xx::Rnd rnd;

	xx::Shared<xx::GLTexture> tex;
	std::vector<xx::Shared<Mouse2>> ms;
	float timePool{}, radians{};
};
