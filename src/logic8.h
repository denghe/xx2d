#pragma once
#include "pch.h"
#include "logic_base.h"

namespace xx {
	struct Node {
		AffineTransform transform;
		XY pos{}, anchor{}, anchorSize{}, size{}, scale{ 1, 1 };
		float radians{};
	};
}


struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

};
