#include "pch.h"

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
