#pragma once
#include "pch.h"

// position ( screen center == 0, 0 )
struct XY {
	float x, y;
	bool operator==(XY const&) const = default;
	bool operator!=(XY const&) const = default;

	inline XY operator+(XY const& o) const { return { x + o.x, y + o.y }; }
	inline XY operator-(XY const& o) const { return { x - o.x, y - o.y }; }
	inline XY operator-() const { return { -x, -y }; }
	inline XY operator*(float const& o) const { return { x * o, y * o }; }
	inline XY operator/(float const& o) const { return { x / o, y / o }; }
};

// texture uv mapping pos
struct UV {
	uint16_t u, v;
};

// 4 bytes color
struct RGBA8 {
	uint8_t r, g, b, a;
	bool operator==(RGBA8 const&) const = default;
	bool operator!=(RGBA8 const&) const = default;
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
struct Size {
	float w, h; 
	bool operator==(Size const&) const = default;
	bool operator!=(Size const&) const = default;

	inline Size operator+(Size const& o) const { return { w + o.w, h + o.h }; }
	inline Size operator-(Size const& o) const { return { w - o.w, h - o.h }; }
	inline Size operator-() const { return { -w, -h }; }
	inline Size operator*(float const& o) const { return { w * o, h * o }; }
	inline Size operator/(float const& o) const { return { w / o, h / o }; }
};

//
struct Rect : XY, Size {};


struct Frame {
	xx::Shared<GLTexture> tex;
	std::string key;
	// std::vector<std::string> aliases;	// unused
	std::optional<XY> anchor;
	XY spriteOffset;
	Size spriteSize;		// content size
	Size spriteSourceSize;	// original pic size
	Rect textureRect;
	bool textureRotated;
	std::vector<uint16_t> triangles;
	std::vector<float> vertices;
	std::vector<float> verticesUV;
};
