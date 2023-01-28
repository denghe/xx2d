#include "pch.h"
#include "logic.h"

void Circle::Init(CircleGrid* const& grid_, int32_t const& x, int32_t const& y, int32_t const& r) {
	assert(!grid);
	assert(!border);
	grid = grid_;
	xy.x = x;
	xy.y = y;
	radius = r;
	grid->Add(this);
	border = std::make_unique<LineStrip>();
	border->FillCirclePoints({ 0,0 }, radius, {}, 12);
	border->SetPositon({ (float)xy.x, (float)-xy.y });
	border->Commit();
}

void Circle::Update(Rnd& rnd) {
	int foreachLimit = Logic4::foreachLimit, numCross{};
	xx::XY<int32_t> v;

	grid->Foreach9NeighborCells<true>(this, [&](Circle* const& c) {
		assert(c != this);
		// fully cross?
		if (c->xy == this->xy) {
			++numCross;
			return;
		}
		// prepare cross check. (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
		auto r12 = (c->radius + this->radius) * (c->radius + this->radius);
		auto p12 = (c->xy.x - this->xy.x) * (c->xy.x - this->xy.x) + (c->xy.y - this->xy.y) * (c->xy.y - this->xy.y);
		// cross?
		if (r12 > p12) {
			auto a = xx::GetAngle<(Logic4::maxDiameter * 2 >= 1024)>(c->xy, this->xy);
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed * r12 / p12, 0 }, a);
			v += inc;
			++numCross;
		}
	}, &foreachLimit);

	// cross?
	if (numCross) {
		auto xy = this->xy;
		// no force: random move?
		if (v.IsZero()) {
			auto a = rnd.Next() % xx::table_num_angles;
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed, 0 }, a);
			xy += inc;
		}
		// move by v?
		else {
			// speed scale limit
			if (numCross > Logic4::speedMaxScale) {
				numCross = Logic4::speedMaxScale;
			}
			auto a = xx::GetAngleXY(v.x, v.y);
			auto inc = xx::Rotate(xx::XY<int32_t>{ Logic4::speed * numCross, 0 }, a);
			xy += inc;
		}

		// 边缘限定( 当前逻辑有重叠才移动，故边界检测放在这里 )
		if (xy.x < 0) xy.x = 0;
		else if (xy.x >= Logic4::maxX) xy.x = Logic4::maxX - 1;
		if (xy.y < 0) xy.y = 0;
		else if (xy.y >= Logic4::maxY) xy.y = Logic4::maxY - 1;

		xy2 = xy;
	} else {
		xy2 = xy;
	}
}

void Circle::Update2() {
	if (xy2 != xy) {
		xy = xy2;
		grid->Update(this);
		border->SetColor({255, 0, 0, 255});
		border->SetPositon({ (float)xy.x, (float)-xy.y });
		border->Commit();
		++grid->numActives;
	}
	else {
		if (border->color != RGBA8{ 255, 255, 255, 255 }) {
			border->SetColor({ 255, 255, 255, 255 });
			border->Commit();
		}
	}
}

Circle::~Circle() {
	if (grid) {
		if (cellIndex > -1) {
			grid->Remove(this);
		}
		grid = {};
	}
}

void CircleGrid::Init(int const& numRows_, int const& numCols_, int const& maxDiameter_) {
	numRows = numRows_;
	numCols = numCols_;
	maxDiameter = maxDiameter_;
	maxY = maxDiameter * numRows;
	maxX = maxDiameter * numCols;
	cells.clear();
	cells.resize(numRows * numCols);
}

void CircleGrid::Add(Circle* const& c) {
	assert(c);
	assert(c->grid == this);
	assert(c->cellIndex == -1);
	assert(!c->prev);
	assert(!c->next);
	assert(c->xy.x >= 0 && c->xy.x < maxX);
	assert(c->xy.y >= 0 && c->xy.y < maxY);

	// calc rIdx & cIdx
	int rIdx = c->xy.y / maxDiameter, cIdx = c->xy.x / maxDiameter;
	int idx = rIdx * numCols + cIdx;
	assert(idx <= cells.size());
	assert(!cells[idx] || !cells[idx]->prev);

	// link
	if (cells[idx]) {
		cells[idx]->prev = c;
	}
	c->next = cells[idx];
	c->cellIndex = idx;
	cells[idx] = c;
	assert(!cells[idx]->prev);
	assert(c->next != c);
	assert(c->prev != c);

	// stat
	++numItems;
}

void CircleGrid::Remove(Circle* const& c) {
	assert(c);
	assert(c->grid == this);
	assert(!c->prev && cells[c->cellIndex] == c || c->prev->next == c && cells[c->cellIndex] != c);
	assert(!c->next || c->next->prev == c);
	//assert(cells[c->cellIndex] include c);

	// unlink
	if (c->prev) {	// isn't header
		assert(cells[c->cellIndex] != c);
		c->prev->next = c->next;
		if (c->next) {
			c->next->prev = c->prev;
			c->next = {};
		}
		c->prev = {};
	} else {
		assert(cells[c->cellIndex] == c);
		cells[c->cellIndex] = c->next;
		if (c->next) {
			c->next->prev = {};
			c->next = {};
		}
	}
	c->cellIndex = -1;
	assert(cells[c->cellIndex] != c);

	// stat
	--numItems;
}

void CircleGrid::Update(Circle* const& c) {
	assert(c);
	//assert(c->grid == this);
	assert(c->cellIndex > -1);
	assert(c->next != c);
	assert(c->prev != c);
	//assert(cells[c->cellIndex] include c);

	auto idx = CalcIndexByPosition(c->xy.x, c->xy.y);
	if (idx == c->cellIndex) return;	// no change
	assert(!cells[idx] || !cells[idx]->prev);
	assert(!cells[c->cellIndex] || !cells[c->cellIndex]->prev);

	// unlink
	if (c->prev) {	// isn't header
		assert(cells[c->cellIndex] != c);
		c->prev->next = c->next;
		if (c->next) {
			c->next->prev = c->prev;
			//c->next = {};
		}
		//c->prev = {};
	} else {
		assert(cells[c->cellIndex] == c);
		cells[c->cellIndex] = c->next;
		if (c->next) {
			c->next->prev = {};
			//c->next = {};
		}
	}
	//c->cellIndex = -1;
	assert(cells[c->cellIndex] != c);
	assert(idx != c->cellIndex);

	// link
	if (cells[idx]) {
		cells[idx]->prev = c;
	}
	c->prev = {};
	c->next = cells[idx];
	cells[idx] = c;
	c->cellIndex = idx;
	assert(!cells[idx]->prev);
	assert(c->next != c);
	assert(c->prev != c);
}

int32_t CircleGrid::CalcIndexByPosition(int32_t const& x, int32_t const& y) {
	assert(x >= 0 && x < maxX);
	assert(y >= 0 && y < maxY);
	int32_t rIdx = y / maxDiameter, cIdx = x / maxDiameter;
	auto idx = rIdx * numCols + cIdx;
	assert(idx <= cells.size());
	return idx;
}




void CircleGridCamera::Init(Size const& screenSize_, CircleGrid* const& cg_) {
	cg = cg_;
	screenSize = screenSize_;
	Commit();
}

void CircleGridCamera::Commit() {
	if (!dirty) return;
	dirty = false;

	auto d = cg->maxDiameter;
	auto fd = (float)d;
	auto halfNumRows = screenSize.h / scale.y / fd / 2;
	int32_t posRowIndex = (int32_t)pos.y / d;
	rowFrom = posRowIndex - halfNumRows;
	rowTo = posRowIndex + halfNumRows + 2;
	if (rowFrom < 0) {
		rowFrom = 0;
	}
	if (rowTo > cg->numRows) {
		rowTo = cg->numRows;
	}

	auto halfNumColumns = screenSize.w / scale.x / fd / 2;
	int32_t posColumnIndex = (int32_t)pos.x / d;
	columnFrom = posColumnIndex - halfNumColumns;
	columnTo = posColumnIndex + halfNumColumns + 2;
	if (columnFrom < 0) {
		columnFrom = 0;
	}
	if (columnTo > cg->numCols) {
		columnTo = cg->numCols;
	}

	offset = { -pos.x, pos.y };
}

void CircleGridCamera::SetScale(float const& scale) {
	this->scale = { scale, scale };
	dirty = true;
}

void CircleGridCamera::SetScreenSize(Size const& wh) {
	this->screenSize = wh;
	dirty = true;
}

void CircleGridCamera::SetPosition(XY const& xy) {
	this->pos = xy;
	dirty = true;
}

void CircleGridCamera::SetPositionX(float const& x) {
	this->pos.x = x;
	dirty = true;
}

void CircleGridCamera::SetPositionY(float const& y) {
	this->pos.y = y;
	dirty = true;
}


void Logic4::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic4 Init( test xy index design performance )" << std::endl;

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
			auto xy = GetMousePosInGrid();
			for (size_t i = 0; i < numEveryInsert; i++) {
				int32_t idx = cs.size();
				auto&& c = cs.emplace_back();
				c.Emplace()->Init(&grid, xy.x, xy.y, rnd.Next(minRadius, maxRadius));
				c->csIndex = idx;
			}
		}

		if (eg->Pressed(Mbtns::Right)) {	// erase
			auto xy = GetMousePosInGrid();
			auto idx = grid.CalcIndexByPosition(xy.x, xy.y);
			// find cross with mouse circle
			grid.Foreach9NeighborCells(idx, [&](Circle* const& c) {
				auto rr = (c->radius + Logic4::maxRadius) * (c->radius + Logic4::maxRadius);	// mouse circle radius == maxRadius
				auto dx = c->xy.x - xy.x;
				auto dy = c->xy.y - xy.y;
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


XY Logic4::GetMousePosInGrid() {
	auto x = cam.pos.x + eg->mousePosition.x / cam.scale.x;
	if (x < 0) x = 0;
	else if (x >= maxX) x = maxX - 1;
	auto y = cam.pos.y - eg->mousePosition.y / cam.scale.y;
	if (y < 0) y = 0;
	else if (y >= maxY) y = maxY - 1;
	return { x, y };
}
