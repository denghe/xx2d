#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic7;
struct C : SpaceGridCItem<C> {
	Logic7* owner{};
	xx::XY<> newPos{};
	int32_t radius{}, speed{ 5 };
	LineStrip border;

	void Init(Logic7* const& owner_, xx::XY<> const& pos, int32_t const& r, int32_t const& segments = 16);
	void SetPos(xx::XY<> const& pos);
	void Update();
	void Update2();
	~C();
};

struct B : SpaceGridABItem<B> {
	Logic7* owner{};
	xx::XY<> size{};
	LineStrip border;

	void Init(Logic7* const& owner_, xx::XY<> const& pos, xx::XY<> const& siz);
	~B();
};

struct Logic7 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	SpaceGridC<C> sgc;
	SpaceGridAB<B> sgab;

	std::vector<C> cs;
	std::vector<B> bs;
	Rnd rnd;
	double timePool{};
};
