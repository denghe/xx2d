#pragma once
#include "pch.h"
#include "logic_base.h"

struct Frame_Anim {
	xx::Shared<Frame> frame;
	xx::Shared<Anim> anim;
};

struct Sprite_Anim {
	xx::Shared<Sprite> sprite;
	Anim* anim{};
};

struct Layer_SAs {
	TMX::Layer_Tile* layer{};
	// mapping to layer.gids
	std::vector<Sprite_Anim> sas;
};

struct Player {
	TP tp;
	Anim anim;
	Sprite sprite;
	bool dirty{ true };
	XY footPos{};
	float yOffset{};	// for display sprite by center
	static constexpr float scale{ 0.25f };	// by res size
	static constexpr int32_t radius{ 48 };	// by res size
};

struct Logic1 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	double timePool{};

	TMX::Camera cam;
	TMX::Map map;
	static constexpr float mapTileLogicAnchorY{ 0.25f };	// by res content "base line"
	float mapTileYOffset{};

	// mapping to map.gidInfos
	std::vector<Frame_Anim> gidFAs;
	// for easy update
	std::vector<Anim*> anims;
	// mapping to "bg".gidInfos
	std::vector<bool> walls;

	Layer_SAs layerBG;	// "bg"
	Layer_SAs layerTrees;	// "trees"
	Player player;
};
