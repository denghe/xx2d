#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic14 : LogicBase {
	xx::Sprite spr;
	void Init(Logic* logic) override;
	int Update() override;
};
