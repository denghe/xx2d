#pragma once
#include "pch.h"

struct Sprite {
	QuadVerts qv;
	union {
		struct {
			uint8_t dirtyFrame;
			uint8_t dirtyPosScaleRotate;
			uint8_t dirtyColor;
			uint8_t dirtyDummy;
		};
		uint32_t dirty = 0xFFFFFFFFu;
	};

	xx::Shared<Frame> frame;

	Size size;
	XY pos;
	XY scale;
	float rotate;
	RGBA8 color;


	void SetTexture(xx::Shared<GLTexture> t);

	void SetTexture(xx::Shared<Frame> f);

	void SetScale(XY const& s);

	void SetPositon(XY const& p);

	void SetColor(RGBA8 const& c);

	// todo: rotate?


	// todo: matrix version
	void Draw(Engine* eg);
};
