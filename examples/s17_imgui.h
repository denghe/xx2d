#pragma once
#include "main.h"

namespace ImGuiTest {

	struct Scene : SceneBase {
		bool showDemoWindow{ true };
		void Init(GameLooper* looper) override;
		int Update() override;
		~Scene();
	};

}
