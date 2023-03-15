#include "pch.h"
#include "scene_points.h"

void Scene_Points::Init(GameLooper* looper_) {
	looper = looper_;
	lsPoint.FillCirclePoints({}, 10, {}, 16);
}

int Scene_Points::Update() {
	if (!line) return 0;

	// todo: edit logic

	// draw
	xx::XY offset{ (looper->leftPanelWidth + looper->margin) / 2, -looper->topPanelHeight / 2 };

	cps.clear();
	for(auto& p : line->points) {
		xx::XY pos{ p.x, p.y };
		cps.emplace_back(pos, (float)p.tension, (int32_t)p.numSegments);
		lsPoint.SetPosition(pos * zoom + offset).Draw();
	}
	mp.Clear();
	mp.FillCurve(line->isLoop, cps);

	// handle error data
	if (mp.totalDistance < 1.f) {
		xx::CoutN(line->name, " data error!");
		line = {};
		return 0;
	}

	xx::MovePathCache mpc;
	mpc.Init(mp);

	auto& lsps = lsBody.SetPoints();
	lsps.clear();
	for (auto& p : mpc.points) {
		lsps.emplace_back(p.pos);
	}
	lsBody.SetPosition(offset).SetScale(zoom).Draw();

	return 0;
}

void Scene_Points::SetLine(MovePathStore::Line* const& line_) {
	if (line == line_) return;
	line = line_;
	if (!line) return;
	// todo: clear points edit state ?
}
