#pragma once
#include "pch.h"

struct Shaders {

	
	inline static constexpr std::string_view vsSrc = R"(#version 330
uniform vec2 uCxy;	// center x y

in vec2 aPos;
in vec3 aTexCoord;
in vec4 aColor;

out vec4 vColor;
out vec3 vTexCoord;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vTexCoord = aTexCoord;
	vColor = aColor;
})"sv;

	inline static constexpr std::string_view fsSrc = R"(#version 330
uniform sampler2D uTex0;

in vec4 vColor;
in vec3 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord.xy / vec2(textureSize(uTex0, 0)));
})"sv;




//	inline static constexpr std::string_view vsSrc = R"(
//uniform vec2 uCxy;	// center x y
//
//attribute vec2 aPos;
//attribute vec3 aTexCoord;
//attribute vec4 aColor;
//
//varying vec4 vColor;
//varying vec3 vTexCoord;
//
//void main() {
//	gl_Position = vec4(aPos / uCxy, 0, 1);
//	vTexCoord = aTexCoord;
//	vColor = aColor;
//})"sv;
//
//	inline static constexpr std::string_view fsSrc = R"(
//uniform sampler2D uTex0;
//
//varying vec4 vColor;
//varying vec3 vTexCoord;
//
//void main() {
//	gl_FragColor = vColor * texture(uTex0, vTexCoord.xy / vec2(textureSize(uTex0, 0)));
//})"sv;

//
//	inline static constexpr std::string_view vsSrc = R"(#version 300 es
//precision highp float;
//uniform vec2 uCxy;	// center x y
//
//in vec2 aPos;
//in vec3 aTexCoord;
//in vec4 aColor;
//
//out mediump vec4 vColor;
//out mediump vec3 vTexCoord;
//
//void main() {
//	gl_Position = vec4(aPos / uCxy, 0, 1);
//	vTexCoord = aTexCoord;
//	vColor = aColor;
//})"sv;
//
//	inline static constexpr std::string_view fsSrc = R"(#version 300 es
//precision mediump float;
//uniform sampler2D uTex0;
//
//in vec4 vColor;
//in vec3 vTexCoord;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor * texture(uTex0, vTexCoord.xy / vec2(textureSize(uTex0, 0)));
//})"sv;
//


//#ifdef GL_ES
};
