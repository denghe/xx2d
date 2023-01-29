#include "pch.h"
#include "logic.h"
#include "logic5.h"

void Foo::Init(SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h) {
	SGABInit(grid);
	SGABSetPosSiz({ x,y }, { w,h });
	SGABAdd();
	SyncBorder();
}

void Foo::SyncBorder() {
	auto hw = float(_sgabHalfSize.x), hh = float(_sgabHalfSize.y);
	border.Emplace();
	border->SetPoints() = { {-hw, hh}, {hw, hh}, {hw, -hh}, {-hw, -hh}, {-hw, hh} };
	border->SetPositon({ (float)_sgabPos.x, -(float)_sgabPos.y });	// flip y to gl coordinate
	border->Commit();
}

Foo::~Foo() {
	this->SGABRemove();
}

void Foo::SetPos(int32_t x, int32_t y) {
	SGABSetPosSiz({ x, y }, _sgabSize);
	SGABUpdate();
	SyncBorder();
}

void Foo::SetPosX(int32_t x) {
	SetPos(x, _sgabPos.y);
}

void Foo::SetPosY(int32_t y) {
	SetPos(_sgabPos.x, y);
}

void Logic5::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic5 Init( test space grid AABB )" << std::endl;

	cam.offset = eg->ninePoints[7];	// (0,0) at top left

	grid.Init(100, 100, 64, 64);

	foos.emplace_back().Emplace()->Init(&grid, 200, 200, 100, 100);
	foos.emplace_back().Emplace()->Init(&grid, 210, 210, 100, 100);
}

int Logic5::Update() {

	timePool += eg->delta;
	auto timePoolBak = timePool;
	while (timePool >= 1.f / 200) {
		timePool -= 1.f / 200;

		auto& f1 = *foos[0];
		if ((eg->Pressed(KbdKeys::Up))) {
			auto y = f1._sgabPos.y - 1;
			if (y - f1._sgabHalfSize.y < 0) y = f1._sgabHalfSize.y;
			f1.SetPosY(y);
		}
		if ((eg->Pressed(KbdKeys::Down))) {
			auto y = f1._sgabPos.y + 1;
			if (y + f1._sgabHalfSize.y >= grid.maxY) y = grid.maxY - f1._sgabHalfSize.y - 1;
			f1.SetPosY(y);
		}
		if ((eg->Pressed(KbdKeys::Left))) {
			auto x = f1._sgabPos.x - 1;
			if (x - f1._sgabHalfSize.x < 0) x = f1._sgabHalfSize.x;
			f1.SetPosX(x);
		}
		if ((eg->Pressed(KbdKeys::Right))) {
			auto x = f1._sgabPos.x + 1;
			if (x + f1._sgabHalfSize.x >= grid.maxX) x = grid.maxX - f1._sgabHalfSize.x - 1;
			f1.SetPosX(x);
		}

	}

	//for (auto& f : foos) {
	//	f->border->Draw(eg, cam);
	//}

	grid.ForeachAABB({ 0,0 }, { 1920,1080 });
	for (auto& f : grid.results) {
		f->border->Draw(eg, cam);
	}
	grid.ClearResults();

	return 0;
}
