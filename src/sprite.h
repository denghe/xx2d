#pragma once
#include "pch.h"

struct Sprite {
	QuadVerts verts;
	xx::Shared<GLTexture> tex;
	XY tsizMscaleD2;

	GLsizei& TW() const;
	GLsizei& TH() const;

	void SetTexture(xx::Shared<GLTexture> t_);	// todo: set uv
	void SetScale(XY const& xy);
	void SetPositon(XY const& xy);
	void SetColor(RGBA8 const& c);
};
