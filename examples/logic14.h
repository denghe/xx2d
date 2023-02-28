#pragma once
#include "xx2d_pch.h"
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
	void Init(GameLooper* looper) override;
	int Update() override;
};
