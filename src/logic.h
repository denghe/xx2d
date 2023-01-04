#pragma once
#include "pch.h"
#include "tmx.h"

// todo: pixel move
struct TmxCamera {
	Rect gridRect;
	Size spriteSize;
	XY spriteScale;

	void SetRange(Engine* eg, TMX::Map& map, Rect const& rect);
	void SetPos(XY const& xy);
};

struct Logic : Engine {
	double secs = 0;
	TmxCamera cam;
	TMX::Map map;
	std::vector<Sprite> ss;


	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
