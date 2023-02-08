#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic7;
struct C : xx::SpaceGridCItem<C> {
	Logic7* owner{};
	xx::Pos<> newPos{};
	int32_t radius{}, speed{ 7 };
	xx::LineStrip border;

	void Init(Logic7* const& owner_, xx::Pos<> const& pos, int32_t const& r, int32_t const& segments = 16);
	void SetPos(xx::Pos<> const& pos);
	void Update();
	void Update2();
	~C();
};

struct B : xx::SpaceGridABItem<B> {
	Logic7* owner{};
	xx::Pos<> size{};
	xx::LineStrip border;

	void Init(Logic7* const& owner_, xx::Pos<> const& pos, xx::Pos<> const& siz);
	~B();
};

struct Logic7 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	xx::SpaceGridC<C> sgc;
	xx::SpaceGridAB<B> sgab;
	xx::SpaceGridCCamera<C> cam;
	xx::Pos<> mousePos;	// store eg->mousePosition's grid world coordinate

	std::vector<xx::Shared<C>> cs;
	std::vector<xx::Shared<B>> bs;
	xx::Rnd rnd;
	double timePool{};
};
