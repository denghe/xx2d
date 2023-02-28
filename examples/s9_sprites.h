#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace Sprites {

	struct Scene;
	struct Mouse {
		xx::Sprite body;
		xx::XY baseInc{};
		void Init(Scene* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color = { 255,255,255,255 });
		int Update();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Shared<xx::GLTexture> tex;
		std::vector<xx::Shared<Mouse>> ms;
		float timePool{}, radians{};
	};

}
