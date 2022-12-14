#pragma once
#include "pch.h"
#include "engine.h"

struct Logic : Engine {
	Label title;
	std::vector<Sprite> ss;
	Rnd rnd;

	void Init();
	void Update(float delta);
};
