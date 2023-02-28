#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"

struct GameLooper {
	xx::BMFont fnt1;
	xx::Label lbInfo;
	std::string extraInfo;
	int fps{}, fpsCounter{};
	double fpsTimePool{}, timePool{};

	xx::Shared<LogicBase> lg;

	template<typename LT>
	void DelaySwitchTo() {
		xx::engine.DelayExecute([this] {
			lg = xx::Make<LT>();
			lg->Init(this);
		});
	}

	void Init();
	int Update();
};
