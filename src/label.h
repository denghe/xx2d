#pragma once
#include "pch.h"
#include "bmfont.h"

struct Label {
	std::vector<QuadVerts> qvs;
	xx::Shared<GLTexture> tex;
	XY tsizMscaleD2;

	void SetText(BMFont& bmf, float const& fontSize, std::string_view const& txt);
	// todo: more set text for ttf?

	void SetScale(XY const& xy);
	void SetPositon(XY const& xy);
	void SetColor(RGBA8 const& c);

	void Draw(Engine* eg);
};
