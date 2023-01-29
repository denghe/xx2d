#pragma once
#include "pch.h"
#include "logic_base.h"
#include "spacegridab.h"

struct Foo : SpaceGridABItem<Foo> {

	xx::Shared<LineStrip> border;

	void Init(SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h);

	void SyncBorder();

	void SetPos(int32_t x, int32_t y);
	void SetPosX(int32_t x);
	void SetPosY(int32_t y);

	~Foo();
};

struct Logic5 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;
	double timePool{};
	Translate cam;
	SpaceGridAB<Foo> grid;
	std::vector<xx::Shared<Foo>> foos;
};
