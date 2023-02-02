#include "pch.h"
#include "logic.h"
#include "logic6.h"

// b: box    c: circle    w: width    h: height    r: radius
// if intersect, cx & cy will be changed & return true
template<typename T = int32_t>
bool MoveCircleIfIntersectsBox1(T const& bx, T const& by, T const& brw, T const& brh, T& cx, T& cy, T const& cr) {
	auto dx = std::abs(cx - bx);
	if (dx > brw + cr) return false;

	auto dy = std::abs(cy - by);
	if (dy > brh + cr) return false;

	if (dx <= brw || dy <= brh) {
		// change cx or cy
		if (brw - dx > brh - dy) {
			if (by > cy) {
				cy = by - brh - cr - 1;
			} else {
				cy = by + brh + cr + 1;
			}
		} else {
			if (bx > cx) {
				cx = bx - brw - cr - 1;
			} else {
				cx = bx + brw + cr + 1;
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

bool MoveCircleIfIntersectsBox1(DragBox& b, DragCircle& c) {
	return MoveCircleIfIntersectsBox1(b.pos.x, b.pos.y, b.size.x / 2, b.size.y / 2, c.pos.x, c.pos.y, c.radius);
}

void Logic6::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic6 Init( test box + circle collision detect )" << std::endl;

	//for (auto i = 0; i < 1; ++i) {
	//	auto r = rnd.Next(16, 100);
	//	XY v{ float(rnd.Get() % ((int)eg->w - r * 2)) + r - eg->hw, float(rnd.Get() % ((int)eg->h - r*2)) + r - eg->hh };
	//	cs.emplace_back().Init(this, v, r, 16);
	//}

	bs.emplace_back().Init({0, 0}, {200, 100});
	bs.emplace_back().Init({150, 50}, {100, 200});
	cs.emplace_back().Init(this, { 300, 300 }, 50, 16);


	BL.Init(eg, Mbtns::Left);
	CL.Init(eg, Mbtns::Right);
}

int Logic6::Update() {

	timePool += eg->delta;
	auto timePoolBak = timePool;
	if (timePool >= 1.f / 60) {
		timePool = 0;

		{
			CL.Update();
			auto&& iter = cs.begin();
			while (CL.eventId && iter != cs.end()) {
				CL.Dispatch(&*iter++);
			}
			if (draggingC) {
				auto tar = eg->mousePosition + draggingC->dxy;
				auto d = tar - draggingC->pos;
				if (d.x > 50) d.x = 50;
				else if (d.x < -50) d.x = -50;
				if (d.y > 50) d.y = 50;
				else if (d.y < -50) d.y = -50;
				draggingC->pos = draggingC->pos + d;
				draggingC->border.SetPositon(draggingC->pos);
				draggingC->border.Commit();
			}
		}
		{
			BL.Update();
			auto&& iter = bs.begin();
			while (BL.eventId && iter != bs.end()) {
				BL.Dispatch(&*iter++);
			}
		}

		for (auto& b : bs) {
			for (auto& c : cs) {
				if (MoveCircleIfIntersectsBox1(b, c)) {
					assert(!MoveCircleIfIntersectsBox1(b, c));
					c.border.SetPositon(c.pos);
					c.border.Commit();
				}
			}
		}
	}

	for (auto& c : cs) {
		c.border.Draw(eg);
	}
	for (auto& b : bs) {
		b.border.Draw(eg);
	}

	return 0;
}


bool DragCircle::HandleMouseDown(LT& L) {
	auto d = pos - L.downPos;
	if (d.x * d.x + d.y * d.y < radiusPow2) {
		dxy = d;
		owner->draggingC = this;
		return true;
	}
	return false;
}
int DragCircle::HandleMouseMove(LT& L) {
	return 0;
}
void DragCircle::HandleMouseUp(LT& L) {
	owner->draggingC = {};
}

void DragCircle::Init(Logic6* const& owner, XY const& pos, float const& radius, int32_t const& segments) {
	this->owner = owner;
	this->pos = pos;
	this->radius = radius;
	this->radiusPow2 = radius * radius;

	border.FillCirclePoints({ 0,0 }, radius, {}, segments);
	border.SetColor({ 255, 255, 0, 255 });
	border.SetPositon(pos);
	border.Commit();
}


bool DragBox::HandleMouseDown(LT& L) {
	auto minXY = pos - size / 2;
	auto maxXY = pos + size / 2;
	return L.downPos.x >= minXY.x && L.downPos.x <= maxXY.x && L.downPos.y >= minXY.y && L.downPos.y <= maxXY.y;
}
int DragBox::HandleMouseMove(LT& L) {
	pos = pos + (L.eg->mousePosition - L.lastPos);
	border.SetPositon(pos);
	border.Commit();
	return 0;
}
void DragBox::HandleMouseUp(LT& L) {}

void DragBox::Init(XY const& pos, XY const& size) {
	this->pos = pos;
	this->size = size;

	border.FillBoxPoints({}, size);
	border.SetColor({ 0, 255, 0, 255 });
	border.SetPositon(pos);
	border.Commit();
}
