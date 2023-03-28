#pragma once
#include "main.h"

namespace XxmvTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Mv mv;
	};
}
