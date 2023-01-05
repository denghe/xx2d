#pragma once
#include "pch.h"

struct Logic : Engine {
	double secs = 0;
	TMX::Camera cam;
	TMX::Map map;
	std::map<TMX::Layer_Tile*, std::vector<Sprite>> tileLayers;

	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
