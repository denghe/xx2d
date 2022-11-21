#include "pch.h"
#include "logic.h"

inline static auto vsSrc = R"(#version 300 es
layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aPosition;
out vec4 vColor;
void main() {
   vColor = aColor;
   gl_Position = aPosition;
})"sv;

inline static auto fsSrc = R"(#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oFragColor;
void main() {
   oFragColor = vColor;
})"sv;

inline static std::array<GLfloat, 4> color = {
	1.0f, 0.0f, 0.0f, 1.0f
};
inline static std::array<GLfloat, 9> verts = {
	0.0f,   0.5f,  0.0f,
	-0.5f, -0.5f,  0.0f,
	0.5f,  -0.5f,  0.0f
};

void Logic::GLInit() {
	v = LoadVertexShader({ vsSrc });
	f = LoadFragmentShader({ fsSrc });
	p = LinkProgram(v, f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);								CheckGLError();
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);	// 防止 glViewport 出错
	glViewport(0, 0, w, h);												CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT);										CheckGLError();
	assert(p);
	glUseProgram(p);													CheckGLError();
	glVertexAttrib4fv(0, color.data());									CheckGLError();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, verts.data());	CheckGLError();
	glEnableVertexAttribArray(1);										CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, 3);									CheckGLError();
}
