#include "pch.h"

namespace xx {
	const double pi2 = 3.14159265358979323846 * 2;

	// 设定计算坐标系 x, y 值范围 为 正负 table_xy_range
	const int table_xy_range = 2048, table_xy_range2 = table_xy_range * 2, table_xy_rangePOW2 =
		table_xy_range * table_xy_range;

	// 角度分割精度。256 对应 uint8_t，65536 对应 uint16_t, ... 对于整数坐标系来说，角度继续细分意义不大。增量体现不出来
	const int table_num_angles = 65536;
	using table_angle_element_type = uint16_t;

	// 查表 sin cos 整数值 放大系数
	const int64_t table_sincos_ratio = 10000;

	// 构造一个查表数组，下标是 +- table_xy_range 二维坐标值。内容是以 table_num_angles 为切割单位的自定义角度值( 并非 360 或 弧度 )
	inline std::array<table_angle_element_type, table_xy_range2* table_xy_range2> table_angle;

	// 基于 table_num_angles 个角度，查表 sin cos 值 ( 通常作为移动增量 )
	inline std::array<int, table_num_angles> table_sin;
	inline std::array<int, table_num_angles> table_cos;


	// 程序启动时自动填表
	struct TableFiller {
		TableFiller() {
			// todo: 多线程计算提速
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

	inline TableFiller tableFiller__;

	// 传入坐标，返回角度值( 0 ~ table_num_angles )  safeMode: 支持大于查表尺寸的 xy 值 ( 慢几倍 )
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

	// 计算点旋转后的坐标
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
}
