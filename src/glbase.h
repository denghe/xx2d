#pragma once
#include "pch.h"

struct GLBase {
	// 外部赋值
	GLsizei w = 1280, h = 720;

	// 内部变量
	Shader v, f;
	Program p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	VertexArrays va;
	Buffer vb, ib;

	// 各种合批参数
	static const size_t maxVertNums = 65536;
	static const size_t maxQuadNums = maxVertNums / 4;
	static const size_t maxIndexNums = maxQuadNums * 6;

	std::pair<GLuint, int> autoBatchTexs[maxQuadNums];	// tex id + count
	size_t autoBatchTexsCount = 0;
	GLuint autoBatchLastTextureId = 0;

	QuadVerts autoBatchQuadVerts[maxQuadNums];
	size_t autoBatchQuadVertsCount = 0;

	void AutoBatchBegin();
	void AutoBatchDrawQuad(Texture& tex, QuadVerts const& qvs);
	void AutoBatchCommit();
	
	// 初始化 opengl 环境
	void GLInit();

	// logic update 之前调用
	void GLUpdateBegin();

	// logic update 之后调用
	void GLUpdateEnd();
};
