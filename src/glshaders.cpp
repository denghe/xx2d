#pragma once
#include "pch.h"

std::string_view Shaders::vsSrc = R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// center x y

in vec2 aPos;
in vec2 aTexCoord;
in vec4 aColor;

out mediump vec4 vColor;
out mediump vec2 vTexCoord;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vTexCoord = aTexCoord;
	vColor = aColor;
})"sv;

std::string_view Shaders::fsSrc = R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv;

//#ifdef GL_ES
