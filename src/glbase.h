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
	//VertexArrays va;
	Buffer vb, ib;

	static const size_t maxVertNums = 65536;
	static const size_t maxIndexNums = maxVertNums * 6 / 4;

	XYUVIJRGBA8 verts[maxVertNums];
	inline static GLushort idxs[maxIndexNums];
	inline static int idxsFiller = [] {
		for (size_t i = 0; i < maxVertNums / 4; i++) {
			auto p = idxs + i * 6;
			auto v = i * 4;
			p[0] = uint16_t(v + 0);
			p[1] = uint16_t(v + 1);
			p[2] = uint16_t(v + 2);
			p[3] = uint16_t(v + 0);
			p[4] = uint16_t(v + 2);
			p[5] = uint16_t(v + 3);
		}
		return 0;
	}();

	size_t autoBatchMaxQuadNums = maxVertNums / 6, autoBatchNumQuads = 0;
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
