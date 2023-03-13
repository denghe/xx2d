#pragma once
#include "main.h"

namespace RenderTextureTest {

	struct RenderTexture {
		xx::Shared<xx::GLTexture> tex;
		void Init(xx::Pos<> wh);
		void Begin();
		void End();
	};

	struct Scene : SceneBase {
		RenderTexture rt;
		xx::Sprite spr, spr2;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
