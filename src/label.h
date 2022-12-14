#pragma once
#include "pch.h"
#include "bmfont.h"

struct Label {
	struct Char {
		//int charId;
		xx::Shared<GLTexture> tex;
		QuadVerts qv;
	};
	std::vector<Char> chars;
	XY lastSize{};
	XY lastPos{};

	// default anchor: 0, 1
	void SetText(BMFont& bmf, std::string_view const& txt, float const& fontSize = 32.f);

	void SetColor(RGBA8 const& c);

	// call after SetText
	void SetPositon(XY const& pos);

	// call after SetPositon
	void SetAnchor(XY const& a);

	void Draw(Engine* eg);
};
