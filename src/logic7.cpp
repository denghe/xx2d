#include "pch.h"
#include "logic.h"
#include "logic7.h"

void C::Init(Logic7* const& owner_, Pos<> const& pos, int32_t const& r, int32_t const& segments) {
	owner = owner_;
	radius = r;

	SGCInit(&owner->sgc);
	SGCSetPos(pos);
	SGCAdd();

	border.FillCirclePoints({ 0,0 }, radius, {}, segments);
	border.SetColor({ 255, 255, 0, 255 });
	border.SetPositon({(float)pos.x, (float)pos.y});
	border.Commit();
}
void C::SetPos(Pos<> const& pos) {
	SGCSetPos(pos);
	SGCUpdate();

	border.SetPositon({ (float)pos.x, (float)pos.y });
	border.Commit();
}
void C::Update() {
	int foreachLimit = 12, numCross{};
	XY v{};	// combine force vector
	newPos = _sgcPos;

	// calc v
	_sgc->Foreach9NeighborCells<true>(this, [&](C* const& c) {
		assert(c != this);
		// fully cross?
		if (c->_sgcPos == _sgcPos) {
			++numCross;
			return;
		}
		// prepare cross check. (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
		auto rrPow2 = (c->radius + this->radius) * (c->radius + this->radius);
		auto d = c->_sgcPos - _sgcPos;
		auto dPow2 = d.x * d.x + d.y * d.y;
		// cross?
		if (rrPow2 > dPow2) {
			auto dxy = std::sqrt(float(dPow2));
			v += d / dxy;
			++numCross;
		}
	}, &foreachLimit);

	// cross?
	if (numCross) {
		if (v.IsZero()) {	// move by random angle
			newPos += Calc::Rotate(Pos<>{ speed, 0 }, owner->rnd.Next() % Calc::table_num_angles);
		}
		else {	// move by v
			newPos += v.Normalize() * speed;
		}

		// todo: get box & fix newPos

		// map edge limit
		if (newPos.x < 0) newPos.x = 0;
		else if (newPos.x >= _sgc->maxX) newPos.x = _sgc->maxX - 1;
		if (newPos.y < 0) newPos.y = 0;
		else if (newPos.y >= _sgc->maxY) newPos.y = _sgc->maxY - 1;
	}
}
void C::Update2() {

}
C::~C() {
	SGCRemove();
}


void B::Init(Logic7* const& owner_, Pos<> const& pos, Pos<> const& siz) {
	owner = owner_;
	size = siz;

	SGABInit(&owner->sgab);
	SGABSetPosSiz(pos, siz);
	SGABAdd();

	border.FillBoxPoints({}, {(float)siz.x, (float)siz.y});
	border.SetColor({ 0, 255, 0, 255 });
	border.SetPositon({ (float)pos.x, (float)pos.y });
	border.Commit();
}
B::~B() {
	SGABRemove();
}


void Logic7::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic7 Init( test box + more circle move to cursor collision detect )" << std::endl;

	//for (auto i = 0; i < 1; ++i) {
	//	auto r = rnd.Next(16, 100);
	//	XY v{ float(rnd.Get() % ((int)eg->w - r * 2)) + r - eg->hw, float(rnd.Get() % ((int)eg->h - r*2)) + r - eg->hh };
	//	cs.emplace_back().Init(this, v, r, 16);
	//}

	//XY wh{ 100, 100 };
	//cs.emplace_back().Init(this, { -400, 400 }, 25, 48);
	//cs.emplace_back().Init(this, { 0, 400 }, 50, 48);
	//cs.emplace_back().Init(this, { 400, 400 }, 90, 48);

	//bs.emplace_back().Init({ 100, 0 }, wh);
	//bs.emplace_back().Init({ 200, 0 }, wh);
	//bs.emplace_back().Init({ 200, 100 }, wh);

	//bs.emplace_back().Init({ -200, 0 }, wh);
	//bs.emplace_back().Init({ -200, 100 }, wh);

	//bs.emplace_back().Init({ -500, 0 }, wh);
}

int Logic7::Update() {
	timePool += eg->delta;
	auto timePoolBak = timePool;
	if (timePool >= 1.f / 60) {
		timePool = 0;

		// todo: move circles to mouse pos ?
	}

	for (auto& c : cs) {
		c.border.Draw(eg);
	}
	for (auto& b : bs) {
		b.border.Draw(eg);
	}

	return 0;
}
