#include "pch.h"
#include "logic.h"
#include "logic7.h"

// b: box    c: circle    w: width    h: height    r: radius
// if intersect, cx & cy will be changed & return true
template<typename T = int32_t>
bool MoveCircleIfIntersectsBox(T const& bx, T const& by, T const& brw, T const& brh, T& cx, T& cy, T const& cr) {
	auto dx = std::abs(cx - bx);
	if (dx > brw + cr) return false;

	auto dy = std::abs(cy - by);
	if (dy > brh + cr) return false;

	if (dx <= brw || dy <= brh) {
		if (brw - dx > brh - dy) {
			if (by > cy) {
				cy = by - brh - cr - 1;	// top
			} else {
				cy = by + brh + cr + 1;	// bottom
			}
		} else {
			if (bx > cx) {
				cx = bx - brw - cr - 1;	// left
			} else {
				cx = bx + brw + cr + 1;	// right
			}
		}
		return true;
	}

	auto dx2 = dx - brw;
	auto dy2 = dy - brh;
	if (dx2 * dx2 + dy2 * dy2 <= cr * cr) {
		// change cx & cy
		auto incX = dx2, incY = dy2;
		float dSeq = dx2 * dx2 + dy2 * dy2;
		if (dSeq == 0.0f) {
			incX = brw + cr * (1.f / 1.414213562373095f) + 1;
			incY = brh + cr * (1.f / 1.414213562373095f) + 1;
		} else {
			auto d = std::sqrt(dSeq);
			incX = brw + cr * dx2 / d + 1;
			incY = brh + cr * dy2 / d + 1;
		}

		if (cx < bx) {
			incX = -incX;
		}
		if (cy < by) {
			incY = -incY;
		}
		cx = bx + incX;
		cy = by + incY;

		return true;
	}
	return false;
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

void C::Init(Logic7* const& owner, XY const& pos, float const& radius, int32_t const& segments) {
	this->owner = owner;
	this->pos = pos;
	this->radius = radius;
	this->radiusPow2 = radius * radius;

	border.FillCirclePoints({ 0,0 }, radius, {}, segments);
	border.SetColor({ 255, 255, 0, 255 });
	border.SetPositon(pos);
	border.Commit();
}

void B::Init(XY const& pos, XY const& size) {
	this->pos = pos;
	this->size = size;

	border.FillBoxPoints({}, size);
	border.SetColor({ 0, 255, 0, 255 });
	border.SetPositon(pos);
	border.Commit();
}
