#pragma once
#include "pch.h"

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

struct Logic;
struct Logic2 {
	Logic* eg{};

	std::vector<DragableCircle> circles;
	DragableCircleMouseEventListener meListener;

	void Init(Logic* eg);
	int Update();
};
