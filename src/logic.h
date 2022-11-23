#pragma once
#include "pch.h"
#include "glbase.h"

struct Logic : GLBase {
	std::vector<Sprite> ss;
	Rnd rnd;

	void Init();
	void Update(float delta);
};
