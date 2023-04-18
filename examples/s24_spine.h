#pragma once
#include "main.h"
#include <xx2d_spine.h>

namespace SpineTest {
	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;
		xx::Coros coros;
		spine::XxPlayer spShared;
	};
}
