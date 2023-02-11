#include "pch.h"
#include "logic.h"
#include "logic4.h"

void Circle::Init(xx::SpaceGridC<Circle>* const& grid_, int32_t const& x, int32_t const& y, int32_t const& r) {
	assert(!_sgc);
	assert(!border);
	_sgc = grid_;
	_sgcPos.x = x;
	_sgcPos.y = y;
	_sgc->Add(this);

	radius = r;
	border = std::make_unique<xx::LineStrip>();
	border->FillCirclePoints({ 0,0 }, radius, {}, Logic4::numCircleSegments).SetPosition({ (float)_sgcPos.x, (float)-_sgcPos.y });
}

void Circle::Update(xx::Rnd& rnd) {
	int foreachLimit = Logic4::foreachLimit, numCross{};
	xx::Pos<> v{};

	_sgc->Foreach9NeighborCells<true>(this, [&](Circle* const& c) {
		assert(c != this);
		auto& cxy = c->_sgcPos, txy = this->_sgcPos;
		// fully cross?
		if (cxy == txy) {
			++numCross;
			return;
		}
		// prepare cross check. (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
		auto r12 = (c->radius + this->radius) * (c->radius + this->radius);
		auto p12 = (cxy.x - txy.x) * (cxy.x - txy.x) + (cxy.y - txy.y) * (cxy.y - txy.y);
		// cross?
		if (r12 > p12) {
			auto a = xx::GetAngle<(Logic4::maxDiameter * 2 >= 1024)>(cxy, txy);
			auto inc = xx::Rotate(xx::Pos<>{ Logic4::speed * r12 / p12, 0 }, a);
			v += inc;
			++numCross;
		}
	}, &foreachLimit);

	// cross?
	if (numCross) {
		auto pos = this->_sgcPos;
		// no force: random move?
		if (v.IsZero()) {
			auto a = rnd.Next() % xx::table_num_angles;
			auto inc = xx::Rotate(xx::Pos<>{ Logic4::speed, 0 }, a);
			pos += inc;
		}
		// move by v?
		else {
			// speed scale limit
			if (numCross > Logic4::speedMaxScale) {
				numCross = Logic4::speedMaxScale;
			}
			auto a = xx::GetAngleXY(v.x, v.y);
			auto inc = xx::Rotate(xx::Pos<>{ Logic4::speed * numCross, 0 }, a);
			pos += inc;
		}

		// map edge limit
		if (pos.x < 0) pos.x = 0;
		else if (pos.x >= Logic4::maxX) pos.x = Logic4::maxX - 1;
		if (pos.y < 0) pos.y = 0;
		else if (pos.y >= Logic4::maxY) pos.y = Logic4::maxY - 1;

		newPos = pos;
	} else {
		newPos = _sgcPos;
	}
}

void Circle::Update2() {
	if (_sgcPos != newPos) {
		_sgcPos = newPos;
		_sgc->Update(this);
		border->SetColor({255, 0, 0, 255})
			.SetPosition({ (float)_sgcPos.x, (float)-_sgcPos.y });
		++_sgc->numActives;
	}
	else {
		if (border->color != xx::RGBA8{ 255, 255, 255, 255 }) {
			border->SetColor({ 255, 255, 255, 255 });
		}
	}
}

Circle::~Circle() {
	if (_sgc) {
		if (_sgcIdx > -1) {
			_sgc->Remove(this);
		}
		_sgc = {};
	}
}



void Logic4::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic4 Init( test spacegrid )" << std::endl;

	grid.Init(numRows, numCols, maxDiameter);

	cs.reserve(capacity);
	for (size_t i = 0; i < numRandCircles; i++) {
		auto&& c = cs.emplace_back();
		c.Emplace()->Init(&grid, rnds[0].Next(0, maxX - 1), rnds[0].Next(0, maxY - 1), rnds[0].Next(minRadius, maxRadius));
		c->csIndex = i;
	}
	//cs.emplace_back().Emplace()->Init(&grid, maxX / 2, maxY / 2, maxRadius);

	cam.Init({ xx::engine.w, xx::engine.h }, &grid);
	cam.SetPosition({ maxX / 2, maxY / 2 });
	cam.SetScale(0.5);

#ifdef ENABLE_MULTITHREAD_UPDATE
	ttp.Init(NUM_UPDATE_THREADS);
#   ifndef THREAD_POOL_USE_RUN_ONCE
	ttp.Start();
#   endif
#endif
}

int Logic4::Update() {
	auto& eg = xx::engine;

	// fixed fps
	timePool += eg.delta;
	if (timePool >= 1.f / 60) {
		timePool = 0;

		xx::XY camInc{ 10 / cam.scale.x, 10 / cam.scale.y };
		float mX = grid.maxX, mY = grid.maxY;
		if ((eg.Pressed(xx::KbdKeys::W))) {
			auto y = cam.pos.y - camInc.y;
			cam.SetPositionY(y < 0 ? 0 : y);
		}
		if ((eg.Pressed(xx::KbdKeys::S))) {
			auto y = cam.pos.y + camInc.y;
			cam.SetPositionY(y >= mY ? (mY - std::numeric_limits<float>::epsilon()) : y);
		}
		if ((eg.Pressed(xx::KbdKeys::A))) {
			auto x = cam.pos.x - camInc.x;
			cam.SetPositionX(x < 0 ? 0 : x);
		}
		if ((eg.Pressed(xx::KbdKeys::D))) {
			auto x = cam.pos.x + camInc.x;
			cam.SetPositionX(x >= mX ? (mX - std::numeric_limits<float>::epsilon()) : x);
		}
		if (eg.Pressed(xx::KbdKeys::Z)) {
			auto x = cam.scale.x + 0.01f;
			cam.SetScale(x < 100 ? x : 100);
		}
		if (eg.Pressed(xx::KbdKeys::X)) {
			auto x = cam.scale.x - 0.01f;
			cam.SetScale(x > 0.001 ? x : 0.001);
		}
		if (eg.Pressed(xx::KbdKeys::C)) {
			cs.clear();
		}

		if (eg.Pressed(xx::Mbtns::Left)) {	// insert
			auto xy = cam.GetMousePosInGrid(eg.mousePosition);
			for (size_t i = 0; i < numEveryInsert; i++) {
				int32_t idx = cs.size();
				auto&& c = cs.emplace_back();
				c.Emplace()->Init(&grid, xy.x, xy.y, rnds[0].Next(minRadius, maxRadius));
				c->csIndex = idx;
			}
		}

		if (eg.Pressed(xx::Mbtns::Right)) {	// erase
			auto pos = cam.GetMousePosInGrid(eg.mousePosition);
			auto idx = grid.CalcIndexByPosition(pos.x, pos.y);
			// find cross with mouse circle
			grid.Foreach9NeighborCells(idx, [&](Circle* const& c) {
				auto rr = (c->radius + Logic4::maxRadius) * (c->radius + Logic4::maxRadius);	// mouse circle radius == maxRadius
				auto dx = c->_sgcPos.x - pos.x;
				auto dy = c->_sgcPos.y - pos.y;
				auto dd = dx * dx + dy * dy;
				if (dd < rr) {	// cross
					tmpcs.push_back(c);
				}
			});
			for(auto& c : tmpcs) {
				// swap remove
				cs.back()->csIndex = c->csIndex;
				cs[c->csIndex] = std::move(cs.back());
				cs.pop_back();
			}
			tmpcs.clear();
		}

		// update physics

		auto secs = xx::NowSteadyEpochSeconds();
#ifdef ENABLE_MULTITHREAD_UPDATE
		auto total = cs.size();
		auto step = total / NUM_UPDATE_THREADS;
		int32_t to = step + total - step * NUM_UPDATE_THREADS;
		ttp.AddTo(0, [&cs = this->cs, &rnd = rnds[0], from = 0, to = to] {
			for (int i = from; i < to; ++i) {
				cs[i]->Update(rnd);
			}
		});
		for (int i = 1; i < NUM_UPDATE_THREADS; i++) {
			ttp.AddTo(i, [&cs = this->cs, &rnd = rnds[i], from = to, to = to + step] {
				for (int i = from; i < to; ++i) {
					cs[i]->Update(rnd);
				}
			});
			to += step;
		}
#   ifdef THREAD_POOL_USE_RUN_ONCE
		ttp.RunOnce();
#   else
		ttp.Go();
#   endif
#else
		for (auto& c : cs) {
			c->Update(rnds[0]);
		}
#endif

		// assign new pos
		grid.numActives = 0;
		for (auto& c : cs) {
			c->Update2();
		}

		// output log
		logic->extraInfo = xx::ToString(", numCircles = ",cs.size(), ", numActives = ", grid.numActives, ", update elapsed secs = ", xx::NowSteadyEpochSeconds(secs));
	}

	// draw
	cam.Commit();
	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			grid.Foreach(rIdx, cIdx, [&](Circle* const& c) {
				c->border->Draw(cam.at);
			});
		}
	}

	return 0;
}
