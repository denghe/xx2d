#pragma once
#include "pch.h"
#include "glhelpers.h"
#include "sprite.h"

struct GLBase {
	// 外部赋值
	GLsizei w = 1280, h = 720;

	// 内部变量
	Shader v, f;
	Program p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	Buffer vb, ib;

	inline static const size_t batchSize = std::numeric_limits<uint16_t>::max() / 6;
	QuadVerts* autoBatchBuf = nullptr, *autoBatchBufEnd = nullptr;
	GLint autoBatchTextureId = -1;
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
