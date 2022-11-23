#pragma once
#include "pch.h"
#include "glbase.h"

struct Logic : GLBase {
	xx::Shared<Texture> t;
	std::vector<Sprite> ss;

	void Init();
	void Update(float delta);
};
