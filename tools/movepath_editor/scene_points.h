#pragma once
#include "main.h"

struct Scene_Points {
	void Init(GameLooper* looper_);
	int Update();

	GameLooper* looper{};
	xx::LineStrip lsBody, lsPoint;
	xx::MovePath mp;
	std::vector<xx::CurvePoint> cps;
	MovePathStore::Line* line{};
	float zoom{ 0.5f };
	void SetLine(MovePathStore::Line* const& line);
	// todo: more cmd func
};
