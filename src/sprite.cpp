#include "pch.h"
#include "sprite.h"

std::string_view Sprite::vsSrc = R"(#version 300 es
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

std::string_view Sprite::fsSrc = R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv;

GLsizei& Sprite::TW() const { return std::get<1>(tex->vs); }
GLsizei& Sprite::TH() const { return std::get<2>(tex->vs); }

void Sprite::SetTexture(xx::Shared<Texture> t_) {
	tex = std::move(t_);
	verts[0].u = 0;       verts[0].v = TH();
	verts[1].u = 0;       verts[1].v = 0;
	verts[2].u = TW();    verts[2].v = 0;
	verts[3].u = TW();    verts[3].v = TH();
}
// todo: set uv

void Sprite::SetScale(XY const& xy) {
	tsizMscaleD2.x = TW() * xy.x / 2.f;
	tsizMscaleD2.y = TH() * xy.y / 2.f;
}

void Sprite::SetPositon(XY const& xy) {
	verts[0].x = verts[1].x = xy.x - tsizMscaleD2.x;
	verts[2].x = verts[3].x = xy.x + tsizMscaleD2.x;
	verts[0].y = verts[3].y = xy.y - tsizMscaleD2.y;
	verts[1].y = verts[2].y = xy.y + tsizMscaleD2.y;
}

void Sprite::SetColor(RGBA8 const& c) {
	memcpy(&verts[0].r, &c, sizeof(c));
	memcpy(&verts[1].r, &c, sizeof(c));
	memcpy(&verts[2].r, &c, sizeof(c));
	memcpy(&verts[3].r, &c, sizeof(c));
}
