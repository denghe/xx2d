#pragma once
#include "pch.h"

struct Logic : Engine {
	double secs = 0;
	TMX::Camera cam;
	TMX::Map map;
	std::vector<Sprite> ss;


	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
