#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic {
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
