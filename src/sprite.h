#pragma once
#include "pch.h"
#include "glhelpers.h"

struct Sprite {
	std::array<XYUVRGBA8, 4> verts;
	xx::Shared<Texture> tex;
	XY tsizMscaleD2;

	GLsizei& TW() const;
	GLsizei& TH() const;

	void SetTexture(xx::Shared<Texture> t_);	// todo: set uv
	void SetScale(XY const& xy);
	void SetPositon(XY const& xy);
	void SetColor(RGBA8 const& c);
};
