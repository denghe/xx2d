#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace PolygonSpriteTest {

	struct Scene;
	struct Tree {
		xx::PolygonSprite body;
		xx::XY baseInc{};
		void Init(Scene* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color = { 255,255,255,255 });
		int Update();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Shared<xx::Frame> f;
		std::vector<xx::Shared<Tree>> ts;
		float timePool{}, radians{};
	};

}
