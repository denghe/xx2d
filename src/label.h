#pragma once
#include "pch.h"

struct Label {
	struct Char {
		xx::Shared<GLTexture> tex;
		QuadVerts qv;
		std::array<XY, 4> posBak;
	};
	std::vector<Char> chars;
	Size maxSize;

	union {
		struct {
			uint8_t dirtyTextSizeAnchorPosScaleRotate;
			uint8_t dirtyColor;
			uint8_t dirtyDummy1;
			uint8_t dirtyDummy2;
		};
		uint32_t dirty = 0xFFFFFFFFu;
	};

	XY pos{ 0, 0 };
	XY anchor{ 0.5, 0.5 };
	XY scale{ 1, 1 };
	float rotate{ 0 };
	RGBA8 color{ 255, 255, 255, 255 };

	// default anchor: 0, 1
	void SetText(BMFont bmf, std::string_view const& text, float const& fontSize = 32.f);

	void SetAnchor(XY const& a);

	void SetRotate(float const& r);

	void SetScale(XY const& s);

	void SetPositon(XY const& p);

	void SetColor(RGBA8 const& c);


	// todo: matrix version
	void Draw(Engine* eg);
};
