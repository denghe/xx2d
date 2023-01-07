#pragma once
#include "pch.h"

struct Frame_Anim {
	xx::Shared<Frame> frame;
	xx::Shared<Anim> anim;
};

struct Sprite_Anim {
	xx::Shared<Sprite> sprite;
	Anim* anim = nullptr;
};

struct Layer_SAs {
	TMX::Layer_Tile* layer;
	std::vector<Sprite_Anim> sas;
};

struct Player {
	TP tp;
	Anim anim;
	Sprite sprite;
	bool dirty = true;
	XY pos{};
};

struct Logic : Engine {
	double secs = 0;

	TMX::Camera cam;
	TMX::Map map;

	// mapping to map.gidInfos
	std::vector<Frame_Anim> gidFAs;
	// for easy update
	std::vector<Anim*> anims;

	// mapping to layer.gids
	std::vector<Layer_SAs> lsass;

	Player player;

	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
