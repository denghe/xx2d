﻿#pragma once
#include "main.h"

namespace Node {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::SpriteNode node;
	};

}
