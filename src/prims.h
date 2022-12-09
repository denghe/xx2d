#pragma once
#include "pch.h"

struct XY {
	float x, y;
};
struct UV {
	uint16_t u, v;
};
struct RGBA8 {
	uint8_t r, g, b, a;
};
struct XYUVRGBA8 : XY, UV, RGBA8 {};

using QuadVerts = std::array<XYUVRGBA8, 4>;
