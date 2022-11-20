#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Logic {
	Shader v, f;
	Program p;
	GLint uMVPMatrix = -1, uTex0 = -1;
	VertexArrays va;
	Buffer b1, b2;
	Texture t;

	Logic();
	void Update();
};
