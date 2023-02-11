#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic11 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::Shared<xx::GLTexture> tex;
	xx::Quad q1, q2;
};
