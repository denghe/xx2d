#include "pch.h"

std::string_view Shaders::vsSrc = R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// center x y

in vec2 aPos;
in vec2 aTexCoord;
in vec4 aColor;

out vec4 vColor;
out vec2 vTexCoord;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vTexCoord = aTexCoord;
	vColor = aColor;
})"sv;

std::string_view Shaders::fsSrc = R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv;


void ShaderManager::Init() {
	// set gl global env for all shaders
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(65535);
	glPointSize(5);

	// make all
	shaders[Shader_XyUvC::index] = xx::Make<Shader_XyUvC>();
	shaders[Shader_XyC::index] = xx::Make<Shader_XyC>();
	// ... more make here

	// init all
	for (auto& s : shaders) {
		s->Init(this);
	}

	// set default
	cursor = Shader_XyUvC::index;
}

void ShaderManager::Begin() {
	drawCall = drawQuads = drawLines = 0;
	shaders[cursor]->Begin();
}

void ShaderManager::Commit() {
	shaders[cursor]->Commit();
}
