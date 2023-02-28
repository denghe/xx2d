#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"

struct Logic8 : LogicBase {
	void Init(GameLooper* looper) override;
	int Update() override;

	xx::SpriteNode node;
};
