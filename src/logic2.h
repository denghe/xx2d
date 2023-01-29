#pragma once
#include "pch.h"
#include "logic_base.h"

struct DragableCircle;
using DragableCircleMouseEventListener = MouseEventListener<DragableCircle*>;

struct DragableCircle : LineStrip {
	using LineStrip::LineStrip;
	bool HandleMouseDown(DragableCircleMouseEventListener& L);
	int HandleMouseMove(DragableCircleMouseEventListener& L);
	void HandleMouseUp(DragableCircleMouseEventListener& L);
	void Init(XY const& pos, float const& radius, int const& segments);
	float radius_square{};
};

struct Logic2 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<DragableCircle> circles;
	DragableCircleMouseEventListener meListener;
};
