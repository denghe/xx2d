#pragma once
#include "pch.h"
#include "engine.h"

struct Logic : Engine {
	Rnd rnd;
	std::vector<std::pair<Sprite, Label>> objs;

	void Init();
	void Update(float delta);
};
