#include "pch.h"
#include "glbase.h"
#include "shaders.h"

void GLBase::GLInit() {
	v = LoadVertexShader({ Shaders::vsSrc });
	f = LoadFragmentShader({ Shaders::fsSrc });
	p = LinkProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");
	aColor = glGetAttribLocation(p, "aColor");

	// todo: 贴图数组支持。绘制前需要先用要用到的贴图，填进数组。Texture 对象将附带存储 其对应的 下标。用的时候 顶点数据 安排一个 贴图下标 的存储位置? 还是说必须配合 draw instance 方案, 再用一个 buffer 存每个实例用哪个 tex 下标？

	glGenBuffers(1, &vb.Ref());
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite::verts) * maxBatchSize, nullptr, GL_DYNAMIC_DRAW);	// GL_STREAM_DRAW
	glBindBuffer(GL_ARRAY_BUFFER, 0);																						CheckGLError();

	glGenBuffers(1, &ib.Ref());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6 * maxBatchSize, nullptr, GL_STATIC_DRAW);
	auto buf = (uint16_t*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * 6 * maxBatchSize, GL_MAP_WRITE_BIT/* | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT*/);	// | GL_MAP_UNSYNCHRONIZED_BIT
	for (size_t i = 0; i < maxBatchSize; i++) {
		buf[i * 6 + 0] = uint16_t(i * 4 + 0);
		buf[i * 6 + 1] = uint16_t(i * 4 + 1);
		buf[i * 6 + 2] = uint16_t(i * 4 + 2);
		buf[i * 6 + 3] = uint16_t(i * 4 + 0);
		buf[i * 6 + 4] = uint16_t(i * 4 + 2);
		buf[i * 6 + 5] = uint16_t(i * 4 + 3);
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);																					CheckGLError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GLBase::GLUpdateBegin() {
	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(p);																										CheckGLError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	AutoBatchBegin();
}

void GLBase::AutoBatchBegin() {
	autoBatchBuf = (QuadVerts*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Sprite::verts) * autoBatchSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
	autoBatchBufEnd = autoBatchBuf + autoBatchSize;
}

void GLBase::AutoBatchDrawQuad(Texture& tex, QuadVerts const& qvs) {
	if (autoBatchTextureId != tex) {
		if (autoBatchTextureId != -1) {
			AutoBatchCommit();
			AutoBatchBegin();
		}
		autoBatchTextureId = tex;
	}
	else if (autoBatchBuf == autoBatchBufEnd) {
		AutoBatchCommit();
		AutoBatchBegin();
	}
	memcpy(autoBatchBuf, qvs.data(), sizeof(QuadVerts));
	++autoBatchBuf;
};

void GLBase::AutoBatchCommit() {
	glUnmapBuffer(GL_ARRAY_BUFFER);																							CheckGLError();

	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);												CheckGLError();
	glEnableVertexAttribArray(aPos);																						CheckGLError();
	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));	CheckGLError();
	glEnableVertexAttribArray(aTexCoord);																					CheckGLError();
	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));		CheckGLError();
	glEnableVertexAttribArray(aColor);																						CheckGLError();

	glUniform2f(uCxy, w / 2, h / 2);																						CheckGLError();

	glActiveTexture(GL_TEXTURE0);																							CheckGLError();
	glBindTexture(GL_TEXTURE_2D, autoBatchTextureId);																		CheckGLError();
	glUniform1i(uTex0, 0);																									CheckGLError();

	glDrawElements(GL_TRIANGLES, (GLsizei)(autoBatchSize - (autoBatchBufEnd - autoBatchBuf)) * 6, GL_UNSIGNED_SHORT, 0);	CheckGLError();
}

void GLBase::GLUpdateEnd() {
	if (autoBatchTextureId != -1) {
		AutoBatchCommit();
	} else {
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
}
