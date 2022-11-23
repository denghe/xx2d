#include "pch.h"
#include "logic.h"

void Logic::GLInit() {
	v = LoadVertexShader({ Sprite::vsSrc });
	f = LoadFragmentShader({ Sprite::fsSrc });
	p = LinkProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");
	aColor = glGetAttribLocation(p, "aColor");

	glGenBuffers(1, &vb.Ref());
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite::verts) * batchSize, nullptr, GL_DYNAMIC_DRAW);	// GL_STREAM_DRAW
	glBindBuffer(GL_ARRAY_BUFFER, 0);																						CheckGLError();

	glGenBuffers(1, &ib.Ref());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6 * batchSize, nullptr, GL_STATIC_DRAW);
	auto buf = (uint16_t*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * 6 * batchSize, GL_MAP_WRITE_BIT/* | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT*/);	// | GL_MAP_UNSYNCHRONIZED_BIT
	for (size_t i = 0; i < batchSize; i++) {
		buf[i * 6 + 0] = uint16_t(i * 4 + 0);
		buf[i * 6 + 1] = uint16_t(i * 4 + 1);
		buf[i * 6 + 2] = uint16_t(i * 4 + 2);
		buf[i * 6 + 3] = uint16_t(i * 4 + 3);
		buf[i * 6 + 4] = uint16_t(i * 4 + 2);
		buf[i * 6 + 5] = uint16_t(i * 4 + 1);
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);																					CheckGLError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	t = xx::Make<Texture>(LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm"));
	size_t numSprites = 10000;
	ss.resize(numSprites);
	for (size_t i = 0; i < numSprites; i++) {
		auto& s = ss[i];
		s.SetTexture(t);
		s.SetScale({ 1, 1 });
		s.SetPositon({ float((rand() % w) - w/2) , float((rand() % h) - h/2) });
		s.SetColor({ uint8_t(rand() % 256), uint8_t(rand() % 256), uint8_t(rand() % 256), 255});
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);																									CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT);																							CheckGLError();

	glUseProgram(p);																										CheckGLError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	auto buf = (decltype(Sprite::verts)*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Sprite::verts) * ss.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
	for (size_t n = ss.size(), i = 0; i < n; ++i) {
		auto& s = ss[i];
		s.SetPositon({ float((rand() % w) - w / 2) , float((rand() % h) - h / 2) });
		memcpy(buf + i, s.verts.data(), sizeof(Sprite::verts));
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);																							CheckGLError();

	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);												CheckGLError();
	glEnableVertexAttribArray(aPos);																						CheckGLError();
	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));	CheckGLError();
	glEnableVertexAttribArray(aTexCoord);																					CheckGLError();
	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));		CheckGLError();
	glEnableVertexAttribArray(aColor);																						CheckGLError();

	glUniform2f(uCxy, w / 2, h / 2);																						CheckGLError();

	glActiveTexture(GL_TEXTURE0);																							CheckGLError();
	glBindTexture(GL_TEXTURE_2D, *t);																						CheckGLError();
	glUniform1i(uTex0, 0);																									CheckGLError();

	glDrawElements(GL_TRIANGLES, (GLsizei)ss.size() * 6, GL_UNSIGNED_SHORT, 0);												CheckGLError();
}
