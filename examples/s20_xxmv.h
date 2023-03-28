#pragma once
#include "main.h"

namespace XxmvTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Mv mv;
		xx::Quad spr;
		std::vector<xx::Shared<xx::GLTexture>> texs;
		int cursor{};
		float timePool{};
	};
}
