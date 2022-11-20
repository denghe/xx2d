#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Logic {
	xx::Shared<Shader> vs, fs;
	xx::Shared<Program> ps;
	xx::Shared<VertexArrays> vao;
	xx::Shared<Buffer> vbo1, vbo2;
	std::vector<xx::Shared<Texture>> ts;

	Logic();
	void Update();
};
