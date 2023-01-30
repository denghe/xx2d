#pragma once
#include "pch.h"
#include "logic_base.h"

#define thisDragableDeriveType ((DragableDeriveType*)(this))

template<typename DragableDeriveType>
struct Dragable {
	using ListenerType = MouseEventListener<DragableDeriveType*>;

	bool HandleMouseDown(ListenerType& L) {
		return thisDragableDeriveType->Inside(L.downPos);
	}
	int HandleMouseMove(ListenerType& L) {
		auto&& pos = thisDragableDeriveType->pos;
		pos = pos + (L.eg->mousePosition - L.lastPos);
		thisDragableDeriveType->SyncDisplay();
		return 0;
	}
	void HandleMouseUp(ListenerType& L) {
	}
};

struct DragCircle : Dragable<DragCircle> {

	void Init(Logic* eg, XY const& pos, float const& radius) {
		this->eg = eg;
		this->pos = pos;
		this->radius = radius;
		this->radiusPow2 = radius * radius;

		border.FillCirclePoints({ 0,0 }, radius, {}, 16);
		border.SetColor({ 255, 255, 0, 255 });
		border.SetPositon(pos);
		border.Commit();
	}

	bool Inside(XY const& tar) {
		auto dx = pos.x - tar.x;
		auto dy = pos.y - tar.y;
		return dx * dx + dy * dy < radiusPow2;
	}

	void SyncDisplay() {
		border.SetPositon(pos);
		border.Commit();
	}

	Logic* eg{};
	XY pos{};
	float radius{}, radiusPow2{};
	LineStrip border;
};

struct Logic6 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;
	Translate cam;
	std::vector<DragCircle> circles;
	typename DragCircle::ListenerType meListener;
};
