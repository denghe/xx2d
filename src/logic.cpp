#include "pch.h"
#include "logic.h"

void Logic::GLInit() {
	s.SetTexture(xx::Make<Texture>(LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm")));
	s.SetScale({ 5, 5 });
	s.SetPositon({ 0, 0 });
	s.SetColor({ 111, 111, 255, 255 });

	v = LoadVertexShader({ Sprite::vsSrc });
	f = LoadFragmentShader({ Sprite::fsSrc });
	p = LinkProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");
	aColor = glGetAttribLocation(p, "aColor");

	glGenBuffers(1, &b.Ref());
	glBindBuffer(GL_ARRAY_BUFFER, b);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s.verts) * 1, nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);																									CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT);																							CheckGLError();

	s.SetPositon({ float((rand() % 100) - 50) , float((rand() % 100) - 50) });

	glUseProgram(p);																										CheckGLError();

	glBindBuffer(GL_ARRAY_BUFFER, b);
	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(s.verts) * 1, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
	// for all sprite
	memcpy(buf, s.verts.data(), sizeof(s.verts));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);												CheckGLError();
	glEnableVertexAttribArray(aPos);																						CheckGLError();
	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));	CheckGLError();
	glEnableVertexAttribArray(aTexCoord);																					CheckGLError();
	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));		CheckGLError();
	glEnableVertexAttribArray(aColor);																						CheckGLError();

	glUniform2f(uCxy, w / 2, h / 2);																						CheckGLError();

	glActiveTexture(GL_TEXTURE0);																							CheckGLError();
	glBindTexture(GL_TEXTURE_2D, *s.tex);																					CheckGLError();
	glUniform1i(uTex0, 0);																									CheckGLError();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																					CheckGLError();
}
