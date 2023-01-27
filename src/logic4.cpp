#include "pch.h"
#include "logic.h"

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
	//assert(c->grid == this);
	assert(c->cellIndex == -1);
	assert(c->prev == nullptr);
	assert(c->next == nullptr);
	assert(c->x >= 0 && c->x < maxX);
	assert(c->y >= 0 && c->y < maxY);

	// calc rIdx & cIdx
	int rIdx = c->y / maxDiameter, cIdx = c->x / maxDiameter;
	int idx = rIdx * numCols + cIdx;
	assert(idx <= cells.size());

	// link
	if (cells[idx]) {
		cells[idx]->prev = c;
		c->next = cells[idx];
	}
	cells[idx] = c;

	// stat
	++numItems;
}

void CircleGrid::Remove(Circle* const& c) {
	assert(c);
	//assert(c->grid == this);
	//assert(cells[c->cellIndex] include c);

	// unlink
	if (c->prev) {
		c->prev->next = c->next;
	} else {
		cells[c->cellIndex] = c->next;
	}
	if (c->next) {
		c->next->prev = c->prev;
	}

	// cleanup
	c->cellIndex = -1;
	c->prev = {};
	c->next = {};

	// stat
	--numItems;
}

void CircleGrid::Update(Circle* const& c) {
	assert(c);
	//assert(c->grid == this);
	assert(c->cellIndex > -1);
	assert(c->x >= 0 && c->x < maxX);
	assert(c->y >= 0 && c->y < maxY);
	//assert(cells[c->cellIndex] include c);

	// calc rIdx & cIdx
	int rIdx = c->y / maxDiameter, cIdx = c->x / maxDiameter;
	int idx = rIdx * numCols + cIdx;
	assert(idx <= cells.size());
	if (idx == c->cellIndex) return;	// no change

	// unlink
	if (c->prev) {
		c->prev->next = c->next;
	} else {
		cells[c->cellIndex] = c->next;
	}
	if (c->next) {
		c->next->prev = c->prev;
	}

	// link
	if (cells[idx]) {
		cells[idx]->prev = c;
		c->next = cells[idx];
	}
	cells[idx] = c;
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

	std::cout << "Logic4 Init( test xy index design )" << std::endl;

	cg.Init(numRows, numCols, maxDiameter);

	cs.resize(10000);
	for (auto& c : cs) {
		//c.grid = &cg;
		c.x = rnd.Next(0, maxX - 1);
		c.y = rnd.Next(0, maxY - 1);
		c.radius = rnd.Next(minRadius, maxRadius);
		cg.Add(&c);

		c.border = std::make_unique<LineStrip>();
		c.border->SetPositon({ (float)c.x, (float)-c.y });
		c.border->FillCirclePoints({ 0,0 }, c.radius, {}, 16);
		c.border->Commit();
	}

	cam.Init({ eg->w, eg->h }, &cg);
}

int Logic4::Update() {
	timePool += eg->delta;
	auto timePoolBak = timePool;
LabBegin:
	// limit control frequency
	if (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		XY camInc{ 10 / cam.scale.x, 10 / cam.scale.y };
		float mX = cg.maxX, mY = cg.maxY;
		if ((eg->Pressed(KbdKeys::Up))) {
			auto y = cam.pos.y - camInc.y;
			cam.SetPositionY(y < 0 ? 0 : y);
		}
		if ((eg->Pressed(KbdKeys::Down))) {
			auto y = cam.pos.y + camInc.y;
			cam.SetPositionY(y >= mY ? (mY - std::numeric_limits<float>::epsilon()) : y);
		}
		if ((eg->Pressed(KbdKeys::Left))) {
			auto x = cam.pos.x - camInc.x;
			cam.SetPositionX(x < 0 ? 0 : x);
		}
		if ((eg->Pressed(KbdKeys::Right))) {
			auto x = cam.pos.x + camInc.x;
			cam.SetPositionX(x >= mX ? (mX - std::numeric_limits<float>::epsilon()) : x);
		}
		if (eg->Pressed(KbdKeys::Z)) {
			auto x = cam.scale.x + 0.001f;
			cam.SetScale(x < 100 ? x : 100);
		}
		if (eg->Pressed(KbdKeys::X)) {
			auto x = cam.scale.x - 0.001f;
			cam.SetScale(x > 0.001 ? x : 0.001);
		}
		cam.Commit();
	}

	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			cg.Foreach(rIdx, cIdx, [&](Circle* const& c) {
				c->border->Draw(eg, cam);
			});
		}
	}

	return 0;
}
