#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"

struct DragableCircle;
using DragableCircleMouseEventListener = xx::MouseEventListener<DragableCircle*>;

struct DragableCircle : xx::LineStrip {
	using xx::LineStrip::LineStrip;
	bool HandleMouseDown(DragableCircleMouseEventListener& L);
	int HandleMouseMove(DragableCircleMouseEventListener& L);
	void HandleMouseUp(DragableCircleMouseEventListener& L);
	void Init(xx::XY const& pos, float const& radius, int const& segments);
	float radius_square{};
};

struct Logic2 : LogicBase {
	void Init(GameLooper* looper) override;
	int Update() override;

	std::vector<DragableCircle> circles;
	DragableCircleMouseEventListener meListener;
};
