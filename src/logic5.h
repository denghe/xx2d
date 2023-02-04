#pragma once
#include "pch.h"
#include "logic_base.h"

struct Foo : SpaceGridABItem<Foo> {

	XY size{}, halfSize{};
	xx::Shared<LineStrip> border;
	RGBA8 color{};

	void Init(SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h, RGBA8 c);

	void SyncBorder();

	void SetPos(int32_t x, int32_t y);
	void SetPosX(int32_t x);
	void SetPosY(int32_t y);

	void MoveUp(int32_t inc);
	void MoveDown(int32_t inc);
	void MoveLeft(int32_t inc);
	void MoveRight(int32_t inc);

	~Foo();
};

struct Logic5 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;
	double timePool{};
	Translate cam;
	SpaceGridAB<Foo> grid;
	std::vector<xx::Shared<Foo>> foos;
	Rnd rnd;
};
