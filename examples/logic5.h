#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"

struct Foo : xx::SpaceGridABItem<Foo> {

	xx::XY size{}, halfSize{};
	xx::Shared<xx::LineStrip> border;
	xx::RGBA8 color{};

	void Init(xx::SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h, xx::RGBA8 c);

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
	void Init(GameLooper* looper) override;
	int Update() override;
	double timePool{};
	xx::AffineTransform cam;
	xx::SpaceGridAB<Foo> grid;
	std::vector<xx::Shared<Foo>> foos;
	xx::Rnd rnd;
};
