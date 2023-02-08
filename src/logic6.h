﻿#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic6;
struct DragCircle {
	using LT = xx::MouseEventListener<DragCircle*>;
	bool HandleMouseDown(LT& L);
	int HandleMouseMove(LT& L);
	void HandleMouseUp(LT& L);

	void Init(Logic6* const& owner, xx::XY const& pos, float const& radius, int32_t const& segments);

	Logic6* owner{};
	xx::XY pos{}, dxy{};
	float radius{}, radiusPow2{};
	xx::LineStrip border;
};

struct DragBox {
	using LT = xx::MouseEventListener<DragBox*>;
	bool HandleMouseDown(LT& L);
	int HandleMouseMove(LT& L);
	void HandleMouseUp(LT& L);

	void Init(xx::XY const& pos, xx::XY const& size);

	Logic6* owner{};
	xx::XY pos{}, size{};
	xx::LineStrip border;
};

struct Logic6 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	std::vector<DragCircle> cs;
	DragCircle::LT CL;
	DragCircle* draggingC{};

	size_t cursor{};
	std::vector<DragBox> bs;
	DragBox::LT BL;
	xx::Rnd rnd;
	double timePool{};
};
