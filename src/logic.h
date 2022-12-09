#pragma once
#include "pch.h"
#include "engine.h"

struct Logic : Engine {
	std::vector<Sprite> ss;
	Rnd rnd;

	void Init();
	void Update(float delta);
};
