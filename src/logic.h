#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Logic {
	// 外部赋值
	GLsizei w = 1280, h = 1024;

	// 内部变量
	Shader v, f;
	Program p;
	GLint uMVPMatrix = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	Buffer b;
	Texture t;

	void GLInit();
	void Update(float delta);
};
