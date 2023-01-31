#pragma once
#include "pch.h"
#include "logic_base.h"

struct DragCircle {
	using LT = MouseEventListener<DragCircle*>;
	bool HandleMouseDown(LT& L) {
		auto dx = pos.x - L.downPos.x;
		auto dy = pos.y - L.downPos.y;
		return dx * dx + dy * dy < radiusPow2;
	}
	int HandleMouseMove(LT& L) {
		pos = pos + (L.eg->mousePosition - L.lastPos);
		border.SetPositon(pos);
		border.Commit();
		return 0;
	}
	void HandleMouseUp(LT& L) {}

	void Init(XY const& pos, float const& radius, int32_t const& segments) {
		this->pos = pos;
		this->prePos = pos;
		this->radius = radius;
		this->radiusPow2 = radius * radius;

		border.FillCirclePoints({ 0,0 }, radius, {}, segments);
		border.SetColor({ 255, 255, 0, 255 });
		border.SetPositon(pos);
		border.Commit();
	}
	XY pos{};
	XY prePos{};
	float radius{}, radiusPow2{};
	LineStrip border;
};

struct DragBox {
	using LT = MouseEventListener<DragBox*>;
	bool HandleMouseDown(LT& L) {
		auto minXY = pos - size / 2;
		auto maxXY = pos + size / 2;
		return L.downPos.x >= minXY.x && L.downPos.x <= maxXY.x && L.downPos.y >= minXY.y && L.downPos.y <= maxXY.y;
	}
	int HandleMouseMove(LT& L) {
		pos = pos + (L.eg->mousePosition - L.lastPos);
		border.SetPositon(pos);
		border.Commit();
		return 0;
	}
	void HandleMouseUp(LT& L) {}

	void Init(XY const& pos, XY const& size) {
		this->pos = pos;
		this->size = size;

		border.FillBoxPoints({}, size);
		border.SetColor({ 0, 255, 0, 255 });
		border.SetPositon(pos);
		border.Commit();
	}
	XY pos{}, size{};
	LineStrip border;
};

struct Logic6 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<DragCircle> cs;
	DragCircle::LT CL;

	std::vector<DragBox> bs;
	DragBox::LT BL;
	Rnd rnd;
};
