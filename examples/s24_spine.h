#pragma once
#include "main.h"

namespace SpineTest {
	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;
		xx::Coros coros;
	};
}
