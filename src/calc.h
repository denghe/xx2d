#include "pch.h"

namespace xx {
	const double pi2 = 3.14159265358979323846 * 2;

	const int table_xy_range = 2048, table_xy_range2 = table_xy_range * 2, table_xy_rangePOW2 =
		table_xy_range * table_xy_range;

	const int table_num_angles = 65536;
	using table_angle_element_type = uint16_t;

	const int64_t table_sincos_ratio = 10000;

	inline std::array<table_angle_element_type, table_xy_range2* table_xy_range2> table_angle;

	inline std::array<int, table_num_angles> table_sin;
	inline std::array<int, table_num_angles> table_cos;

	struct TableFiller {
		TableFiller() {
			for (int y = -table_xy_range; y < table_xy_range; ++y) {
				for (int x = -table_xy_range; x < table_xy_range; ++x) {
					auto idx = (y + table_xy_range) * table_xy_range2 + (x + table_xy_range);
					auto a = atan2((double)y, (double)x);
					if (a < 0) a += pi2;
					table_angle[idx] = (table_angle_element_type)(a / pi2 * table_num_angles);
				}
			}
			// fix same x,y
			table_angle[(0 + table_xy_range) * table_xy_range2 + (0 + table_xy_range)] = 0;

			for (int i = 0; i < table_num_angles; ++i) {
				auto s = sin((double)i / table_num_angles * pi2);
				table_sin[i] = (int)(s * table_sincos_ratio);
				auto c = cos((double)i / table_num_angles * pi2);
				table_cos[i] = (int)(c * table_sincos_ratio);
			}
		}
	};

	inline TableFiller tableFiller__;	// auto fill on startup

	// safeMode: abs(x|y) >= 1024
	template<bool safeMode = true, typename T = int>
	table_angle_element_type GetAngleXY(T x, T y) noexcept {
		if constexpr (safeMode) {
			while (x < -table_xy_range || x >= table_xy_range || y < -table_xy_range || y >= table_xy_range) {
				x /= 8;
				y /= 8;
			}
		} else {
			assert(x >= -table_xy_range && x < table_xy_range&& y >= -table_xy_range && y < table_xy_range);
		}
		return table_angle[(y + table_xy_range) * table_xy_range2 + x + table_xy_range];
	}

	template<bool safeMode = true, typename T = int>
	table_angle_element_type GetAngleXYXY(T const& x1, T const& y1, T const& x2, T const& y2) noexcept {
		return GetAngleXY<safeMode>(x2 - x1, y2 - y1);
	}

	template<bool safeMode = true, typename Point1, typename Point2>
	table_angle_element_type GetAngle(Point1 const& from, Point2 const& to) noexcept {
		return GetAngleXY<safeMode>(to.x - from.x, to.y - from.y);
	}

	template<typename P, typename T = decltype(P::x)>
	inline P Rotate(T const& x, T const& y, table_angle_element_type const& a) noexcept {
		auto s = (int64_t)table_sin[a];
		auto c = (int64_t)table_cos[a];
		return { (T)((x * c - y * s) / table_sincos_ratio), (T)((x * s + y * c) / table_sincos_ratio) };
	}
	template<typename P>
	inline P Rotate(P const& p, table_angle_element_type const& a) noexcept {
		static_assert(std::is_same_v<decltype(p.x), decltype(p.y)>);
		return Rotate<P, decltype(p.x)>(p.x, p.y, a);
	}

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

	// check 2 segments( p0-p1, p2-p3 ) is cross. fill cross point to p
	template<typename Point, typename Point1, typename Point2>
	inline bool GetSegmentIntersection(Point1 const& p0, Point1 const& p1, Point2 const& p2, Point2 const& p3, Point* const& p = nullptr) noexcept {
		Point s02, s10, s32;
		float s_numer, t_numer, denom, t;
		s10.x = p1.x - p0.x;
		s10.y = p1.y - p0.y;
		s32.x = p3.x - p2.x;
		s32.y = p3.y - p2.y;

		denom = s10.x * s32.y - s32.x * s10.y;
		if (denom == 0) return false; // Collinear
		bool denomPositive = denom > 0;

		s02.x = p0.x - p2.x;
		s02.y = p0.y - p2.y;
		s_numer = s10.x * s02.y - s10.y * s02.x;
		if ((s_numer < 0) == denomPositive) return false; // No collision

		t_numer = s32.x * s02.y - s32.y * s02.x;
		if ((t_numer < 0) == denomPositive) return false; // No collision

		if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive)) return false; // No collision

		t = t_numer / denom;        // Collision detected
		if (p) {
			p->x = p0.x + (t * s10.x);
			p->y = p0.y + (t * s10.y);
		}
		return true;
	}
}
