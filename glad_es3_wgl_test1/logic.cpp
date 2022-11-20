#include "pch.h"
#include "logic.h"

Logic::Logic() {
	v = LoadVertexShader({ R"(#version 300 es
precision highp float;
uniform mat4 uMVPMatrix;

in vec4 aPos;
in vec2 aTexCoord;
in vec4 aColor;

out mediump vec4 vFragColor;
out mediump vec2 vTexCoord;

void main() {
    gl_Position = uMVPMatrix * aPos;
	vFragColor = aColor;
	vTexCoord = aTexCoord;
})" });
	f = LoadFragmentShader({ R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vFragColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vFragColor * texture(uTex0, vTexCoord);
}
)" });
	p = LinkProgram(v, f);

	glBindAttribLocation(p, 0, "aPos");
	glBindAttribLocation(p, 1, "aTexCoord");
	glBindAttribLocation(p, 2, "aColor");

	uTex0 = glGetUniformLocation(p, "uTex0");
	assert(uTex0 == 1);
	glUniform1i(uTex0, 0);

	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
	assert(uMVPMatrix == 0);
	//glUniform1i(uMVPMatrix, 0);

	glUseProgram(p);
}

void Logic::Update() {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
