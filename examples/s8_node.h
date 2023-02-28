#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace Node {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::SpriteNode node;
	};

}
