#pragma once
#include "pch.h"

struct Logic;
struct LogicBase {
	Logic* eg{};
	virtual void Init(Logic*) = 0;
	virtual int Update() = 0;
	virtual ~LogicBase() {};
};

#include "logic0.h"
#include "logic1.h"
#include "logic2.h"
#include "logic3.h"
#include "logic4.h"

struct Logic : Engine {
	BMFont fnt1;
	Label lbInfo;
	std::string moreInfo;
	int fps{}, fpsCounter{};
	double nowSecs{}, delta{}, fpsTimePool{}, timePool{};

	xx::Shared<LogicBase> lg;

	template<typename LT>
	void DelaySwitchTo() {
		DelayExecute([this] {
			lg = xx::Make<LT>();
			lg->Init(this);
		});
	}

	void Init();
	int Update();
};
