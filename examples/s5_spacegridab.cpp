#include "main.h"
#include "s5_spacegridab.h"

namespace SpaceGridAB {

	void Foo::Init(xx::SpaceGridAB<Foo>* grid, int32_t x, int32_t y, int32_t w, int32_t h, xx::RGBA8 c) {
		SGABInit(grid);
		SGABSetPosSiz({ x,y }, { w,h });
		SGABAdd();
		color = c;
		size.x = w;
		size.y = h;
		halfSize.x = w / 2;
		halfSize.y = h / 2;
		SyncBorder();
	}

	void Foo::SyncBorder() {
		auto hw = halfSize.x, hh = halfSize.y;
		border.Emplace();
		border->FillBoxPoints({}, size)
			.SetPosition({ (float)_sgabPos.x, -(float)_sgabPos.y })	// flip y to gl coordinate
			.SetColor(color);
	}

	Foo::~Foo() {
		this->SGABRemove();
	}

	void Foo::SetPos(int32_t x, int32_t y) {
		SGABSetPosSiz({ x, y }, { (int32_t)size.x, (int32_t)size.y });
		SGABUpdate();
		SyncBorder();
	}

	void Foo::SetPosX(int32_t x) {
		SetPos(x, _sgabPos.y);
	}

	void Foo::SetPosY(int32_t y) {
		SetPos(_sgabPos.x, y);
	}

	void Foo::MoveUp(int32_t inc) {
		auto y = _sgabPos.y - inc;
		if (y - halfSize.y < 0) y = halfSize.y;
		SetPosY(y);
	}
	void Foo::MoveDown(int32_t inc) {
		auto y = _sgabPos.y + inc;
		if (y + halfSize.y >= _sgab->maxY) y = _sgab->maxY - halfSize.y - 1;
		SetPosY(y);
	}
	void Foo::MoveLeft(int32_t inc) {
		auto x = _sgabPos.x - inc;
		if (x - halfSize.x < 0) x = halfSize.x;
		SetPosX(x);
	}
	void Foo::MoveRight(int32_t inc) {
		auto x = _sgabPos.x + inc;
		if (x + halfSize.x >= _sgab->maxX) x = _sgab->maxX - halfSize.x - 1;
		SetPosX(x);
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "SpaceGridAB::Scene::Init" << std::endl;

		cam = cam.MakePos(xx::engine.ninePoints[7]);	// (0,0) at top left

		grid.Init(50, 50, 256, 256);

		for (size_t i = 0; i < 100000; i++) {
			auto w = rnd.Next(30, 300);
			auto h = rnd.Next(30, 300);
			auto x = rnd.Next(w / 2, grid.maxX - 1 - w / 2);
			auto y = rnd.Next(h / 2, grid.maxY - 1 - h / 2);
			auto c = rnd.Get();
			foos.emplace_back().Emplace()->Init(&grid, x, y, w, h, (xx::RGBA8&)c);
		}
	}

	int Scene::Update() {
		auto& eg = xx::engine;
		timePool += eg.delta;
		auto timePoolBak = timePool;
		if (timePool >= 1.f / 60) {
			timePool = 0;
			constexpr int32_t moveSpeed = 3;
			if ((eg.Pressed(xx::KbdKeys::W))) {
				for (auto& f : foos) f->MoveUp(moveSpeed);
			}
			if ((eg.Pressed(xx::KbdKeys::S))) {
				for (auto& f : foos) f->MoveDown(moveSpeed);
			}
			if ((eg.Pressed(xx::KbdKeys::A))) {
				for (auto& f : foos) f->MoveLeft(moveSpeed);
			}
			if ((eg.Pressed(xx::KbdKeys::D))) {
				for (auto& f : foos) f->MoveRight(moveSpeed);
			}
		}

		// todo: cam control

		grid.ForeachAABB({ 0,0 }, { (int)eg.w,(int)eg.h });
		for (auto& f : grid.results) {
			f->border->Draw(cam);
		}
		looper->extraInfo = xx::ToString(", numAllBoxs = ", foos.size(), ", numScreenBoxs = ", grid.results.size());
		grid.ClearResults();


		return 0;
	}

}
