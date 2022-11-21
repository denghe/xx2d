#include "pch.h"

// some opengl documents put here

/*
* glEnable
* glDisable
* gIisEnabled
*
GL_BLEND
GL_CULL_FACE
GL_DEPTH_TEST
GL_DITHER
GL_POLYGON_OFFSET_FILL
GL_PRIMITIVE_RESTART_FIXED_INDEX
GL_RASTERIZER_DISCARD
GL_SAMPLE_ALPHA_TO_COVERAGE
GL_SAMPLE_COVERAGE
GL_SCISSOR_TEST
GL_STENCIL_TEST
*/


/*

Logic::Logic() {
	v = LoadVertexShader({ R"(#version 300 es
precision highp float;
uniform mat4 uMVPMatrix;

in vec4 aPos;
in vec4 aColor;
in vec2 aTexCoord;

out mediump vec4 vColor;
out mediump vec2 vTexCoord;

void main() {
	gl_Position = uMVPMatrix * aPos;
	vColor = aColor;
	vTexCoord = aTexCoord;
})" });
	f = LoadFragmentShader({ R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord);
}
)" });
	p = LinkProgram(v, f);

	glBindAttribLocation(p, 0, "aPos");
	glBindAttribLocation(p, 1, "aColor");
	glBindAttribLocation(p, 2, "aTexCoord");

	uTex0 = glGetUniformLocation(p, "uTex0");
	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
}


	//glUseProgram(p);

	//glUniform1i(uTex0, 0);
	//glUniform???(uMVPMatrix, 0);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, t);
*/