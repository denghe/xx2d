#include "pch.h"
#include "logic.h"

void Circle::Init(SpaceGrid<Circle>* const& grid_, int32_t const& x, int32_t const& y, int32_t const& r) {
	assert(!grid);
	assert(!border);
	_sgrid = grid_;
	_sgridPos.x = x;
	_sgridPos.y = y;
	_sgrid->Add(this);

	radius = r;
	border = std::make_unique<LineStrip>();
	border->FillCirclePoints({ 0,0 }, radius, {}, 12);
	border->SetPositon({ (float)_sgridPos.x, (float)-_sgridPos.y });
	border->Commit();
}

void Circle::Update(Rnd& rnd) {
	int foreachLimit = Logic4::foreachLimit, numCross{};
	xx::XY<int32_t> v;

	_sgrid->Foreach9NeighborCells<true>(this, [&](Circle* const& c) {
		assert(c != this);
		auto& cxy = c->_sgridPos, txy = this->_sgridPos;
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
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed * r12 / p12, 0 }, a);
			v += inc;
			++numCross;
		}
	}, &foreachLimit);

	// cross?
	if (numCross) {
		auto pos = this->_sgridPos;
		// no force: random move?
		if (v.IsZero()) {
			auto a = rnd.Next() % xx::table_num_angles;
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed, 0 }, a);
			pos += inc;
		}
		// move by v?
		else {
			// speed scale limit
			if (numCross > Logic4::speedMaxScale) {
				numCross = Logic4::speedMaxScale;
			}
			auto a = xx::GetAngleXY(v.x, v.y);
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed * numCross, 0 }, a);
			pos += inc;
		}

		// 边缘限定( 当前逻辑有重叠才移动，故边界检测放在这里 )
		if (pos.x < 0) pos.x = 0;
		else if (pos.x >= Logic4::maxX) pos.x = Logic4::maxX - 1;
		if (pos.y < 0) pos.y = 0;
		else if (pos.y >= Logic4::maxY) pos.y = Logic4::maxY - 1;

		newPos = pos;
	} else {
		newPos = _sgridPos;
	}
}

void Circle::Update2() {
	if (_sgridPos != newPos) {
		_sgridPos = newPos;
		_sgrid->Update(this);
		border->SetColor({255, 0, 0, 255});
		border->SetPositon({ (float)_sgridPos.x, (float)-_sgridPos.y });
		border->Commit();
		++_sgrid->numActives;
	}
	else {
		if (border->color != RGBA8{ 255, 255, 255, 255 }) {
			border->SetColor({ 255, 255, 255, 255 });
			border->Commit();
		}
	}
}

Circle::~Circle() {
	if (_sgrid) {
		if (_sgridIdx > -1) {
			_sgrid->Remove(this);
		}
		_sgrid = {};
	}
}



void Logic4::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic4 Init( test spacegrid )" << std::endl;

	grid.Init(numRows, numCols, maxDiameter);

	cs.reserve(capacity);
	for (size_t i = 0; i < numRandCircles; i++) {
		auto&& c = cs.emplace_back();
		c.Emplace()->Init(&grid, rnd.Next(0, maxX - 1), rnd.Next(0, maxY - 1), rnd.Next(minRadius, maxRadius));
		c->csIndex = i;
	}

	cam.Init({ eg->w, eg->h }, &grid);
	cam.SetPosition({ maxX / 2, maxY / 2 });
	cam.SetScale(0.5);
}

int Logic4::Update() {

	// fixed fps
	timePool += eg->delta;
	while (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		XY camInc{ 10 / cam.scale.x, 10 / cam.scale.y };
		float mX = grid.maxX, mY = grid.maxY;
		if ((eg->Pressed(KbdKeys::W))) {
			auto y = cam.pos.y - camInc.y;
			cam.SetPositionY(y < 0 ? 0 : y);
		}
		if ((eg->Pressed(KbdKeys::S))) {
			auto y = cam.pos.y + camInc.y;
			cam.SetPositionY(y >= mY ? (mY - std::numeric_limits<float>::epsilon()) : y);
		}
		if ((eg->Pressed(KbdKeys::A))) {
			auto x = cam.pos.x - camInc.x;
			cam.SetPositionX(x < 0 ? 0 : x);
		}
		if ((eg->Pressed(KbdKeys::D))) {
			auto x = cam.pos.x + camInc.x;
			cam.SetPositionX(x >= mX ? (mX - std::numeric_limits<float>::epsilon()) : x);
		}
		if (eg->Pressed(KbdKeys::Z)) {
			auto x = cam.scale.x + 0.01f;
			cam.SetScale(x < 100 ? x : 100);
		}
		if (eg->Pressed(KbdKeys::X)) {
			auto x = cam.scale.x - 0.01f;
			cam.SetScale(x > 0.001 ? x : 0.001);
		}

		if (eg->Pressed(Mbtns::Left)) {	// insert
			auto xy = cam.GetMousePosInGrid(eg->mousePosition);
			for (size_t i = 0; i < numEveryInsert; i++) {
				int32_t idx = cs.size();
				auto&& c = cs.emplace_back();
				c.Emplace()->Init(&grid, xy.x, xy.y, rnd.Next(minRadius, maxRadius));
				c->csIndex = idx;
			}
		}

		if (eg->Pressed(Mbtns::Right)) {	// erase
			auto pos = cam.GetMousePosInGrid(eg->mousePosition);
			auto idx = grid.CalcIndexByPosition(pos.x, pos.y);
			// find cross with mouse circle
			grid.Foreach9NeighborCells(idx, [&](Circle* const& c) {
				auto rr = (c->radius + Logic4::maxRadius) * (c->radius + Logic4::maxRadius);	// mouse circle radius == maxRadius
				auto dx = c->_sgridPos.x - pos.x;
				auto dy = c->_sgridPos.y - pos.y;
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
		for (auto& c : cs) {
			c->Update(rnd);
		}
		// assign new pos
		grid.numActives = 0;
		for (auto& c : cs) {
			c->Update2();
		}
		if (grid.numActives) {
			//std::cout << "grid.numActives = " << grid.numActives << std::endl;
		}
	}

	// draw
	cam.Commit();
	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			grid.Foreach(rIdx, cIdx, [&](Circle* const& c) {
				c->border->Draw(eg, cam);
			});
		}
	}

	return 0;
}

