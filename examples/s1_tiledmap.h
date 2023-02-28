#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace TiledMap {

	struct Frame_Anim {
		xx::Shared<xx::Frame> frame;
		xx::Shared<xx::Anim> anim;
	};

	struct Sprite_Anim {
		xx::Shared<xx::Sprite> sprite;
		xx::Anim* anim{};
	};

	struct Layer_SAs {
		xx::TMX::Layer_Tile* layer{};
		// mapping to layer.gids
		std::vector<Sprite_Anim> sas;
	};

	struct Player {
		xx::TP tp;
		xx::Anim anim;
		xx::Sprite sprite;
		bool dirty{ true };
		xx::XY footPos{};
		float yOffset{};	// for display sprite by center
		static constexpr float scale{ 0.25f };	// by res size
		static constexpr int32_t radius{ 48 };	// by res size
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		double timePool{};

		xx::TMX::Camera cam;
		xx::TMX::Map map;
		static constexpr float mapTileLogicAnchorY{ 0.25f };	// by res content "base line"
		float mapTileYOffset{};

		// mapping to map.gidInfos
		std::vector<Frame_Anim> gidFAs;
		// for easy update
		std::vector<xx::Anim*> anims;
		// mapping to "bg".gidInfos
		std::vector<bool> walls;

		Layer_SAs layerBG;	// "bg"
		Layer_SAs layerTrees;	// "trees"
		Player player;
	};

}
