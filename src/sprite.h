#pragma once
#include "pch.h"

struct Sprite {
	QuadVerts qv;
	xx::Shared<GLTexture> tex;
	XY lastPos;
	XY tsizMscaleD2;

	GLsizei& TW() const;
	GLsizei& TH() const;

	void SetTexture(xx::Shared<GLTexture> t_);	// todo: set uv
	void SetScale(XY const& scale);
	void SetPositon(XY const& pos);
	void SetColor(RGBA8 const& c);

	void Draw(Engine* eg);
};
