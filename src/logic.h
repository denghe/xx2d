#pragma once
#include "pch.h"
#include "logicbase.h"

struct Logic : LogicBase {
	std::vector<Sprite> ss;
	Rnd rnd;

	void Init();
	void Update(float delta);
};
