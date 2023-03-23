#pragma once
#include "main.h"

namespace RenderTextureTest {

	struct Scene : SceneBase {
		xx::Shared<xx::GLTexture> tex0;
		xx::Sprite spr, spr2;
		xx::Rnd rnd;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
