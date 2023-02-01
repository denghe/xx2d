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
			incX = brw + cr / 1.414213562373095f + 1;
			incY = brh + cr / 1.414213562373095f + 1;
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

	for (auto i = 0; i < 1000; ++i) {
		auto r = rnd.Next(16, 100);
		XY v{ float(rnd.Get() % ((int)eg->w - r * 2)) + r - eg->hw, float(rnd.Get() % ((int)eg->h - r*2)) + r - eg->hh };
		cs.emplace_back().Init(v, r, 16);
	}

	bs.emplace_back().Init({}, {50, 200});


	BL.Init(eg, Mbtns::Left);
	CL.Init(eg, Mbtns::Right);
}

int Logic6::Update() {

	{
		CL.Update();
		auto&& iter = cs.begin();
		while (CL.eventId && iter != cs.end()) {
			CL.Dispatch(&*iter++);
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

	for (auto& c : cs) {
		c.border.Draw(eg);
		c.prePos = c.pos;
	}
	for (auto& b : bs) {
		b.border.Draw(eg);
	}

	return 0;
}
