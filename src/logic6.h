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
	void HandleMouseUp(LT& L) {
	}

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
		auto& p = L.downPos;
		return p.x >= leftTop.x && p.x <= rightBottom.x
			&& p.y >= leftTop.y && p.y <= rightBottom.y;
	}
	int HandleMouseMove(LT& L) {
		pos = pos + (L.eg->mousePosition - L.lastPos);
		leftTop = pos - hs;
		rightBottom = pos + hs;
		border.SetPositon(pos);
		border.Commit();
		return 0;
	}
	void HandleMouseUp(LT& L) {
	}

	void Init(XY const& pos, XY const& size) {
		this->pos = pos;
		hs = size / 2;
		leftTop = pos - hs;
		rightBottom = pos + hs;

		border.SetPoints() = { {-hs.x,-hs.y},{-hs.x,hs.y},{hs.x,hs.y},{hs.x,-hs.y},{-hs.x,-hs.y} };
		border.SetColor({ 0, 255, 0, 255 });
		border.SetPositon(pos);
		border.Commit();
	}

	float Maxx() { return rightBottom.x; }
	float Minx() { return leftTop.x; }
	float Maxy() { return rightBottom.y; }
	float Miny() { return leftTop.y; }

	XY pos{}, hs{}, leftTop{}, rightBottom{};
	LineStrip border;
};

struct Logic6 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<DragCircle> cs;
	DragCircle::LT CL;

	std::vector<DragBox> bs;
	DragBox::LT BL;
};
