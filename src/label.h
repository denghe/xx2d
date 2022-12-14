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
	XY lastPos{};
	float tsizMscaleD2{};

	void SetText(BMFont& bmf, std::string_view const& txt, float const& fontSize = 32.f, XY const& pos = {});

	//void SetScale(XY const& scale);
	void SetPositon(XY const& pos);

	void SetColor(RGBA8 const& c);

	void Draw(Engine* eg);
};
