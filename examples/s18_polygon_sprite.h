#pragma once
#include "main.h"

namespace PolygonSpriteTest {

	struct Scene;
	struct Sword {
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
		xx::ListLink<Sword> ts;
		float timePool{}, radians{};
	};

}
