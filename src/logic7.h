#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic7;
struct C {
	void Init(Logic7* const& owner, XY const& pos, float const& radius, int32_t const& segments);

	Logic7* owner{};
	XY pos{}, dxy{};
	float radius{}, radiusPow2{};
	LineStrip border;
};

struct B {
	void Init(XY const& pos, XY const& size);

	Logic7* owner{};
	XY pos{}, size{};
	LineStrip border;
};

struct Logic7 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<C> cs;
	std::vector<B> bs;
	Rnd rnd;
	double timePool{};
};
