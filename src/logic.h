#pragma once
#include "pch.h"

struct Sprite_FrameAnim {
	xx::Shared<Sprite> sprite;
	xx::Shared<FrameAnim> frameAnim;
	size_t cursor = 0;
	float timePool = 0;
	void Step();
	bool Update(float const& delta);
	Frame_Duration const& GetCurrentFrame_Duration() const;
	xx::Shared<Frame> const& GetCurrentFrame() const;
};

struct Logic : Engine {
	double secs = 0;

	// todo: move following fields to TMXManager ?
	TMX::Camera cam;
	TMX::Map map;

	// mapping to map.gidInfos for every static tile
	std::vector<xx::Shared<Frame>> gidFrames;

	// mapping to map.gidInfos for tile anim
	std::vector<xx::Shared<FrameAnim>> gidFrameAnims;

	// mapping to layer.gids
	std::map<TMX::Layer_Tile*, std::vector<Sprite_FrameAnim>> tileLayer_Sprites;

	// for sync update
	std::vector<Sprite_FrameAnim*> allSfas;

	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
