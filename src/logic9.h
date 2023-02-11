#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic9;
struct Mouse {
	xx::Sprite body;
	xx::XY baseInc{};
	void Init(Logic9* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color = {255,255,255,255});
	int Update();
	void Draw();
};

struct Logic9 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::Shared<xx::GLTexture> tex;
	std::vector<xx::Shared<Mouse>> ms;
	float timePool{}, radians{};
};
