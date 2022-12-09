#pragma once
#include "pch.h"
#include "engine.h"

struct Logic {
	Engine eg;	// game loop support. can't change name
	std::vector<Sprite> ss;
	Rnd rnd;

	void Init();
	void Update(float delta);
};
