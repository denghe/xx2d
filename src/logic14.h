#pragma once
#include "pch.h"
#include "logic_base.h"

struct Scissor {
	xx::Rect rect;
	void Begin();
	void End();
};

struct Logic14 : LogicBase {
	Scissor scissor;
	xx::Sprite spr;
	xx::Coros coros;
	void Init(Logic* logic) override;
	int Update() override;
};
