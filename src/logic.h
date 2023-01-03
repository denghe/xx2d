#pragma once
#include "pch.h"
#include "tmx.h"

// todo: Camera
struct TmxCamera {
	// sprite add AssignCamera: if cameraVersion != tar  dirty ...
	Rect rect;
	Size size;
	XY scale;

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
