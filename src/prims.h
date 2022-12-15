#pragma once
#include "pch.h"

// position ( screen center == 0, 0 )
struct XY {
	float x, y;
};

// texture uv mapping pos
struct UV {
	uint16_t u, v;
};

// 4 bytes color
struct RGBA8 {
	uint8_t r, g, b, a;
};

// 1 vert data
struct XYUVRGBA8 : XY, UV, RGBA8 {};

/*
 1┌────┐2
  │    │
 0└────┘3
*/
using QuadVerts = std::array<XYUVRGBA8, 4>;

//
struct Size { float w, h; };

//
struct Rect : XY, Size {};


struct Frame {
	xx::Shared<GLTexture> tex;
	Rect rect;
	bool rotated;
};
