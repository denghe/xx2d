#include "pch.h"

void Shader_XyC::Init(ShaderManager* sm) {
	this->sm = sm;
	v = LoadGLVertexShader({ R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// center x y

in vec2 aPos;
in vec4 aColor;

out vec4 vColor;
out vec2 vTexCoord;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vTexCoord = aTexCoord;
	vColor = aColor;
})"sv });

	f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;

in vec4 vColor;

out vec4 oColor;

void main() {
	oColor = vColor;
})"sv });

	p = LinkGLProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");

	aPos = glGetAttribLocation(p, "aPos");
	aColor = glGetAttribLocation(p, "aColor");
	CheckGLError();

}

void Shader_XyC::Begin() {
	// todo
}

void Shader_XyC::Commit() {
	// todo
}

XYRGBA8* Shader_XyC::DrawLineBegin(int numPoints) {
	// todo
	return 0;
}

void Shader_XyC::DrawLineCommit() {
	// todo
}
