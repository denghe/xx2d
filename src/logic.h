#pragma once
#include "pch.h"
#include "glhelpers.h"
#include "sprite.h"

struct Logic {
	// 外部赋值
	GLsizei w = 1280, h = 720;

	// 内部变量
	Shader v, f;
	Program p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	Buffer b;
	
	Sprite s;	//xx::Shared<Texture> t;

	void GLInit();
	void Update(float delta);
};
