#pragma once
#include "main.h"

namespace RenderTextureTest {

	struct Scene : SceneBase {
		xx::Sprite spr, spr2;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
