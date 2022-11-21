#include "pch.h"
#include "logic.h"

inline static auto vsSrc = R"(#version 300 es
precision highp float;
//uniform mat4 uMVPMatrix;

in vec4 aColor;
in vec4 aPos;
in vec2 aTexCoord;

out mediump vec4 vColor;
out mediump vec2 vTexCoord;

void main() {
	vColor = aColor;
	gl_Position = /*uMVPMatrix * */aPos;
	vTexCoord = aTexCoord;
})"sv;

inline static auto fsSrc = R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord);
})"sv;

inline static std::array<GLfloat, 4> color = {
	1.0f, 1.0f, 1.0f, 1.0f,
};
inline static std::array<GLfloat, 20> verts = {
	-0.5f,  -0.5f,  0.0f,/*     */0.0f, 1.0f,
	-0.5f,   0.5f,  0.0f,/*     */0.0f, 0.0f,
	 0.5f,   0.5f,  0.0f,/*     */1.0f, 0.0f,
	 0.5f,  -0.5f,  0.0f,/*     */1.0f, 1.0f,
};

void Logic::GLInit() {
	t = LoadTexture("D:/Codes/xx2d/_bak/res/zazaka.pkm"sv);

	v = LoadVertexShader({ vsSrc });
	f = LoadFragmentShader({ fsSrc });
	p = LinkProgram(v, f);

	//uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aColor = glGetAttribLocation(p, "aColor");
	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");

	glGenBuffers(1, &b.Ref());
	glBindBuffer(GL_ARRAY_BUFFER, b);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), nullptr, GL_STREAM_DRAW);
	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
	memcpy(buf, verts.data(), sizeof(float) * verts.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(p);																							CheckGLError();

	glVertexAttrib4fv(aColor, color.data());																	CheckGLError();

	glBindBuffer(GL_ARRAY_BUFFER, b);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);									CheckGLError();
	glEnableVertexAttribArray(aPos);																			CheckGLError();

	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));	CheckGLError();
	glEnableVertexAttribArray(aTexCoord);																		CheckGLError();

	glActiveTexture(GL_TEXTURE0);																				CheckGLError();
	glBindTexture(GL_TEXTURE_2D, t);																			CheckGLError();
	glUniform1i(uTex0, 0);																						CheckGLError();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																		CheckGLError();
}




//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
//
//in vec4 aColor;
//in vec4 aPos;
//
//out mediump vec4 vColor;
//
//void main() {
//	vColor = aColor;
//	gl_Position = aPos;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//
//in vec4 vColor;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor;
//})"sv;
//
//inline static std::array<GLfloat, 12> color = {
//	1.0f, 0.0f, 0.0f, 1.0f,
//	0.0f, 1.0f, 0.0f, 1.0f,
//	0.0f, 0.0f, 1.0f, 1.0f,
//};
//inline static std::array<GLfloat, 9> verts = {
//	0.0f,   0.5f,  0.0f,
//	-0.5f, -0.5f,  0.0f,
//	0.5f,  -0.5f,  0.0f
//};

//void Logic::GLInit() {
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aColor = glGetAttribLocation(p, "aColor");
//	aPos = glGetAttribLocation(p, "aPos");
//	aTexCoord = glGetAttribLocation(p, "aTexCoord");
//
//	//glEnable(GL_DEPTH_TEST);
//	//glDepthFunc(GL_LEQUAL);
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glUseProgram(p);														CheckGLError();
//	//glVertexAttrib4fv(aColor, color.data());								CheckGLError();
//	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, 0, color.data());	CheckGLError();
//	glEnableVertexAttribArray(aColor);										CheckGLError();
//	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, verts.data());	CheckGLError();
//	glEnableVertexAttribArray(aPos);										CheckGLError();
//	glDrawArrays(GL_TRIANGLES, 0, 3);										CheckGLError();
//}



