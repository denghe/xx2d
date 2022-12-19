#pragma once
#include "pch.h"

struct Monster {
	int64_t id;
	double x, y;			// pos in global map
	double drawOrder;		// -(y + id / 1000000000)
	int64_t hp;
	Sprite sprite;
	Label label;
};

struct Logic : Engine {
	Rnd rnd;
	double timePool{};
	xx::Coros coros;

	std::set<xx::Shared<Monster>> monsters;
	std::vector<Monster*> tmp;

	int64_t objsCounter{};
	xx::Shared<Frame> fZazaka, fMouse;
	BMFont fnt1, fnt2;
	Label lbCount;

	void Init();
	int Update();
};
