#pragma once
#include "pch.h"
#include "logic_base.h"

namespace xx {
	struct Node {
		AffineTransform at;
		XY pos{}, scale{ 1, 1 };
		float radians{};
		std::vector<std::pair<xx::Label, xx::Sprite>> ss;
	};
}

struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::Node n;
	xx::Shared<xx::GLTexture> t;
};
