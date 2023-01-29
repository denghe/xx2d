#include "pch.h"
#include "logic.h"
#include "logic5.h"

void Foo::Init(SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h) {
	this->SGABInit(grid);
	this->SGABSetPosSiz({ x,y }, { w,h });
	this->SGABAdd();

	auto hw = float(w / 2), hh = float(h / 2);
	border.Emplace();
	border->SetPoints() = { {-hw, hh}, {hw, hh}, {hw, -hh}, {-hw, -hh}, {-hw, hh} };
	border->SetPositon({ (float)x, -(float)y });	// flip y to gl coordinate
	border->Commit();
}

void Logic5::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic5 Init( test space grid AABB )" << std::endl;

	cam.offset = eg->ninePoints[7];	// (0,0) at top left

	grid.Init(100, 100, 64, 64);

	foos.emplace_back().Emplace()->Init(&grid, 200, 200, 100, 100);
	foos.emplace_back().Emplace()->Init(&grid, 210, 210, 100, 100);

	foos.pop_back();
}

int Logic5::Update() {

	for (auto& f : foos) {
		f->border->Draw(eg, cam);
	}

	return 0;
}
