#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Logic {
	GLsizei w = 1024, h = 768;

	Shader v, f;
	Program p;
	GLint uMVPMatrix = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	VertexArrays va;
	Buffer b1, b2;
	Texture t;

	Logic();
	void Init();
	void Update();
};
