#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic6 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;
	Translate cam;
};
