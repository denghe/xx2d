#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace Scissor {

	struct GLScissor {
		xx::Rect rect;
		void Begin();
		void End();
	};

	struct Scene : SceneBase {
		GLScissor scissor;
		xx::Sprite spr;
		xx::Coros coros;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
