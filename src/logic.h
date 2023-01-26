#pragma once
#include "pch.h"

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

struct DragableCircle : LineStrip {
	using LineStrip::LineStrip;
	bool HandleMouseDown(MouseEventListener<DragableCircle>* L);
	int HandleMouseMove(MouseEventListener<DragableCircle>* L);
	void HandleMouseUp(MouseEventListener<DragableCircle>* L);
	void Init(XY const& pos, float const& radius, int const& segments);
	float radius_square{};
};

struct Logic : Engine {
	double nowSecs{};
	float timePool{};

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

	std::vector<DragableCircle> circles;
	MouseEventListener<DragableCircle> meListener;

	BMFont fnt1;
	Label lbCount;
	void Init();
	int Update();
};
