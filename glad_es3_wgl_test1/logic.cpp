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
	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
}

void Logic::Update() {
	glUseProgram(p);

	//glUniform1i(uTex0, 0);
	//glUniform???(uMVPMatrix, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
