#pragma once
#include "xx2d.h"
#include "scene_base.h"
#include "xx_coro_simple.h"
#include <xx_threadpool.h>

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase, font3500;
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

	int Init() override;
	int Update() override;
};
