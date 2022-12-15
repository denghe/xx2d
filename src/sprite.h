#pragma once
#include "pch.h"

struct Sprite {
	QuadVerts qv;

	union {
		struct {
			uint8_t dirtyFrame;
			uint8_t dirtySizeAnchorPosScaleRotate;
			uint8_t dirtyColor;
			uint8_t dirtyDummy;
		};
		uint32_t dirty = 0xFFFFFFFFu;
	};

	xx::Shared<Frame> frame;
	XY pos{ 0, 0 };
	XY anchor{ 0.5, 0.5 };
	XY scale{ 1, 1 };
	float rotate{ 0 };
	RGBA8 color{ 255, 255, 255, 255 };


	void SetTexture(xx::Shared<GLTexture> t);

	void SetTexture(xx::Shared<Frame> f);

	void SetAnchor(XY const& a);

	void SetRotate(float const& r);

	void SetScale(XY const& s);

	void SetPositon(XY const& p);

	void SetColor(RGBA8 const& c);

	// todo: rotate?


	// todo: matrix version
	void Draw(Engine* eg);
};
