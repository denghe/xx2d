#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace ImGuiTest {

	struct Scene : SceneBase {
		bool showDemoWindow{ true };
		void Init(GameLooper* looper) override;
		int Update() override;
		~Scene();
	};

}
