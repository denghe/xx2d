#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

struct GameLooper {
	xx::BMFont fnt1;
	xx::SimpleLabel lbInfo;
	std::string extraInfo;
	int fps{}, fpsCounter{};
	double fpsTimePool{}, timePool{};

	xx::Shared<SceneBase> scene;

	template<typename LT>
	void DelaySwitchTo() {
		xx::engine.DelayExecute([this] {
			scene = xx::Make<LT>();
			scene->Init(this);
		});
	}

	void Init();
	int Update();
};
