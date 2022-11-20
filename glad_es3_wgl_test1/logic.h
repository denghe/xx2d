#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Logic {
	Shader v, f;
	Program p;
	VertexArrays va;
	Buffer b1, b2;
	std::vector<xx::Shared<Texture>> ts;

	Logic();
	void Update();
};
