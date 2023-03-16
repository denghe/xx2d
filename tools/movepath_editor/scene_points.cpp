#include "pch.h"
#include "scene_points.h"

void Scene_Points::Init(GameLooper* looper_) {
	looper = looper_;
	lsPoint.FillCirclePoints({}, 10, {}, 16);
}

int Scene_Points::Update() {
	if (!line) return 0;

	timePool += xx::engine.delta;
	while (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		if (xx::engine.Pressed(xx::KbdKeys::Z)) {
			zoom += 0.02f;
			if (zoom >= 2.f) {
				zoom = 2.f;
			}
		}
		if (xx::engine.Pressed(xx::KbdKeys::X)) {
			zoom -= 0.02f;
			if (zoom <= 0.3f) {
				zoom = 0.3f;
			}
		}

		// todo: edit logic
	}

	// draw begin
	xx::XY offset{ (looper->leftPanelWidth + looper->margin) / 2, 0 };
	xx::LineStrip ls;
	ls.SetScale(zoom).SetPosition(offset);

	// draw bg
	float dw = looper->data.designWidth, dh = looper->data.designHeight, sl = looper->data.safeLength;
	float dw2 = dw / 2, dh2 = dh / 2;
	float r1 = std::sqrt(dw2 * dw2 + dh2 * dh2);
	float r2 = r1 + sl;
	float w = dw + sl, h = dh + sl;
	float w2 = w / 2, h2 = h / 2;
	ls.SetColor({255,127,127,255}).SetPoints({ {-r2, 0}, {r2, 0} }).Draw();
	ls.SetPoints({ {0, -r2}, {0, r2} }).Draw();
	ls.SetColor({ 127,127,255,255 }).FillBoxPoints({}, { dw, dh }).Draw();
	ls.SetColor({ 127,255,127,255 }).FillCirclePoints({}, r1 ).Draw();
	ls.SetColor({ 255,255,127,255 }).FillCirclePoints({}, r2 ).Draw();

	// draw points
	cps.clear();
	for(auto& p : line->points) {
		xx::XY pos{ p.x, p.y };
		cps.emplace_back(pos, (float)p.tension, (int32_t)p.numSegments);
		lsPoint.SetPosition(pos * zoom + offset).SetColor({255,255,0,255}).Draw();
	}
	mp.Clear();
	mp.FillCurve(line->isLoop, cps);

	// draw body segments
	if (mp.totalDistance < 0.001f) return 0;

	xx::MovePathCache mpc;
	mpc.Init(mp);

	auto& ps = ls.Clear().SetPoints();
	for (auto& p : mpc.points) {
		ps.emplace_back(p.pos);
	}
	ls.SetColor({ 255,255,255,255 }).SetPosition(offset).SetScale(zoom).Draw();

	xx::SimpleLabel lbl;
	lbl.SetScale({ 0.85, 1 }).SetAnchor({ 0,0 }).SetColor({ 127,127,0,255 })
		.SetPosition(xx::engine.ninePoints[1].MakeAdd(looper->leftPanelWidth + looper->margin * 2, 32 + looper->margin))
		.SetText(looper->fnt, xx::ToString("zoom = ", zoom, ", points.size() = ", line->points.size()))
		.Draw()
		.SetAnchor({ 0,1 })
		.SetPosition(xx::engine.ninePoints[7].MakeAdd(looper->leftPanelWidth + looper->margin * 2, -looper->margin))
		.SetText(looper->fnt, "Z/X: Zooom in/out;  W/S: cursor up/down; C/V: copy/paste; F: clear"sv)
		.Draw();
	return 0;
}

void Scene_Points::SetLine(MovePathStore::Line* const& line_) {
	if (line == line_) return;
	line = line_;
	if (!line) return;
	// todo: clear points edit state ?
}
