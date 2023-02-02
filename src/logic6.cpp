#include "pch.h"
#include "logic.h"
#include "logic6.h"

// todo: limit corner directions only

union AvaliableDirections {
	struct {
		union {
			struct {
				uint8_t top, bottom;
			};
			uint16_t top_bottom;
		};
		union {
			struct {
				uint8_t left, right;
			};
			uint16_t left_right;
		};
	};
	uint32_t all = 0xFFFFFFFFu;
};

// b: box    c: circle    w: width    h: height    r: radius
// if intersect, cx & cy will be changed & return true
template<typename T = int32_t>
bool MoveCircleIfIntersectsBox(T const& bx, T const& by, T const& brw, T const& brh, T& cx, T& cy, T const& cr, AvaliableDirections const& ad = {}) {
	if (!ad.all) return false;

	auto dx = std::abs(cx - bx);
	if (dx > brw + cr) return false;

	auto dy = std::abs(cy - by);
	if (dy > brh + cr) return false;

	if (dx <= brw || dy <= brh) {
		if (ad.all == 0xFFFFFFFFu) {	// all direction avaliable
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
		} else {
			// find short way out
			// store to array. left = 1, right = 2, top = 3, bottom = 4
			std::array<std::pair<int, T>, 4> a;
			size_t asiz{};
			if (ad.left) {
				a[asiz++] = { 1, brw + cr + (bx > cx ? -dx : dx) };
			}
			if (ad.right) {
				a[asiz++] = { 2, brw + cr + (bx > cx ? dx : -dx)};
			}
			if (ad.top) {
				a[asiz++] = { 3, brh + cr + (by > cy ? -dy : dy) };
			}
			if (ad.bottom) {
				a[asiz++] = { 4, brh + cr + (by > cy ? dy : -dy) };
			}

			// find min
			std::pair<int, T> minVal = a[0];
			for (size_t i = 1; i < asiz; ++i) {
				if (a[i].second < minVal.second) {
					minVal = a[i];
				}
			}

			// change cx or cy
			switch (minVal.first) {
			case 1:
				cx = bx - brw - cr - 1;	// left
				return true;
			case 2:
				cx = bx + brw + cr + 1;	// right
				return true;
			case 3:
				cy = by - brh - cr - 1;	// top
				return true;
			case 4:
				cy = by + brh + cr + 1;	// bottom
				return true;
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

		//if (cx < bx) {
		//	incX = -incX;
		//}
		//if (cy < by) {
		//	incY = -incY;
		//}
		//cx = bx + incX;
		//cy = by + incY;

		// change cx cy
		if (by > cy) {	// quadrant: 1, 2
			if (bx < cx) {	// 1
				if (ad.right && ad.top) {
					cx = bx + incX;
					cy = by - incY;
				} else if (ad.right) {
					cx = bx + brw + cr + 1;	// right
				} else {
					cy = by - brh - cr - 1;	// top
				}
			} else {	// 2
				if (ad.left && ad.top) {
					cx = bx - incX;
					cy = by - incY;
				} else if (ad.left) {
					cx = bx - brw - cr - 1;	// left
				} else {
					cy = by - brh - cr - 1;	// top
				}
			}
		} else {	// quadrant: 3, 4
			if (bx < cx) {	// 4
				if (ad.right && ad.bottom) {
					cx = bx + incX;
					cy = by + incY;
				} else if (ad.right) {
					cx = bx + brw + cr + 1;	// right
				} else {
					cy = by + brh + cr + 1;	// bottom
				}
			} else {	// 3
				if (ad.left && ad.bottom) {
					cx = bx - incX;
					cy = by + incY;
				} else if (ad.left) {
					cx = bx - brw - cr - 1;	// left
				} else {
					cy = by + brh + cr + 1;	// bottom
				}
			}
		}

		return true;
	}
	return false;
}

bool MoveCircleIfIntersectsBox(DragBox& b, DragCircle& c, AvaliableDirections const& avaliableDirections = {}) {
	return MoveCircleIfIntersectsBox(b.pos.x, b.pos.y, b.size.x / 2, b.size.y / 2, c.pos.x, c.pos.y, c.radius, avaliableDirections);
}

void Logic6::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic6 Init( test box + circle collision detect )" << std::endl;

	//for (auto i = 0; i < 1; ++i) {
	//	auto r = rnd.Next(16, 100);
	//	XY v{ float(rnd.Get() % ((int)eg->w - r * 2)) + r - eg->hw, float(rnd.Get() % ((int)eg->h - r*2)) + r - eg->hh };
	//	cs.emplace_back().Init(this, v, r, 16);
	//}

	XY wh{ 100, 100 };
	cs.emplace_back().Init(this, { -400, 400 }, 25, 48);
	cs.emplace_back().Init(this, { 0, 400 }, 50, 48);
	cs.emplace_back().Init(this, { 400, 400 }, 90, 48);

	bs.emplace_back().Init({ 100, 0 }, wh);
	bs.emplace_back().Init({ 200, 0 }, wh);
	bs.emplace_back().Init({ 200, 100 }, wh);

	bs.emplace_back().Init({ -200, 0 }, wh);
	bs.emplace_back().Init({ -200, 100 }, wh);

	bs.emplace_back().Init({ -500, 0 }, wh);


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
				auto limit = 50;// draggingC->radius / 2;
				if (d.x > limit) d.x = limit;
				else if (d.x < -limit) d.x = -limit;
				if (d.y > limit) d.y = limit;
				else if (d.y < -limit) d.y = -limit;
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

		
		auto pointInBoxs = [this](XY const& p)->uint8_t {
			for (auto& b : bs) {
				auto hs = b.size / 2;
				auto leftTop = b.pos - hs;
				if (p.x < leftTop.x) continue;
				if (p.y < leftTop.y) continue;
				auto rightBottom = b.pos + hs;
				if (p.x > rightBottom.x) continue;
				if (p.y > rightBottom.y) continue;
				return 0;
			}
			return 0xFFu;
		};

		for (auto& b : bs) {
			//if (!(b.pos.x == 100.f && b.pos.y == 0.f)) continue;
			// fill avaliableDirections
			auto hs = b.size / 2 + 2;	// + 2 : guess point
			AvaliableDirections ads;
			ads.left = pointInBoxs({ b.pos.x - hs.x, b.pos.y });
			ads.right = pointInBoxs({ b.pos.x + hs.x, b.pos.y });
			ads.top = pointInBoxs({ b.pos.x, b.pos.y - hs.y });
			ads.bottom = pointInBoxs({ b.pos.x, b.pos.y + hs.y });

			for (auto& c : cs) {
				if (MoveCircleIfIntersectsBox(b, c, ads)) {
					assert(!MoveCircleIfIntersectsBox(b, c));
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
