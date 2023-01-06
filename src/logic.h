#pragma once
#include "pch.h"

struct Anim {
	FrameAnim fa;
	size_t cursor = 0;
	float timePool = 0;
	void Step();
	bool Update(float const& delta);
	Frame_Duration const& GetCurrentFrame_Duration() const;
	xx::Shared<Frame> const& GetCurrentFrame() const;
};

struct Frame_Anim {
	xx::Shared<Frame> frame;
	xx::Shared<Anim> anim;
};

struct Sprite_Anim {
	xx::Shared<Sprite> sprite;
	Anim* anim = nullptr;
	void Draw(Engine* eg, TMX::Camera& cam);
};
using SAs = std::vector<Sprite_Anim>;

struct Layer_SAs {
	TMX::Layer_Tile* layer;
	SAs sas;
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


	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
