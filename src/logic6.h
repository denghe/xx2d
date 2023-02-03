#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic6;
struct DragCircle {
	using LT = MouseEventListener<DragCircle*>;
	bool HandleMouseDown(LT& L);
	int HandleMouseMove(LT& L);
	void HandleMouseUp(LT& L);

	void Init(Logic6* const& owner, XY const& pos, float const& radius, int32_t const& segments);

	Logic6* owner{};
	XY pos{}, dxy{};
	float radius{}, radiusPow2{};
	LineStrip border;
};

struct DragBox {
	using LT = MouseEventListener<DragBox*>;
	bool HandleMouseDown(LT& L);
	int HandleMouseMove(LT& L);
	void HandleMouseUp(LT& L);

	void Init(XY const& pos, XY const& size);

	Logic6* owner{};
	XY pos{}, size{};
	LineStrip border;
};

struct Logic6 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<DragCircle> cs;
	DragCircle::LT CL;
	DragCircle* draggingC{};

	size_t cursor{};
	std::vector<DragBox> bs;
	DragBox::LT BL;
	Rnd rnd;
	double timePool{};
};
