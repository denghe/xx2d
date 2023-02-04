#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic7;
struct C : SpaceGridCItem<C> {
	Logic7* owner{};
	Pos<> newPos{};
	int32_t radius{}, speed{ 5 };
	LineStrip border;

	void Init(Logic7* const& owner_, Pos<> const& pos, int32_t const& r, int32_t const& segments = 16);
	void SetPos(Pos<> const& pos);
	void Update();
	void Update2();
	~C();
};

struct B : SpaceGridABItem<B> {
	Logic7* owner{};
	Pos<> size{};
	LineStrip border;

	void Init(Logic7* const& owner_, Pos<> const& pos, Pos<> const& siz);
	~B();
};

struct Logic7 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	SpaceGridC<C> sgc;
	SpaceGridAB<B> sgab;
	SpaceGridCCamera<C> cam;
	Pos<> mousePos;	// store eg->mousePosition's grid world coordinate

	std::vector<xx::Shared<C>> cs;
	std::vector<xx::Shared<B>> bs;
	Rnd rnd;
	double timePool{};
};
