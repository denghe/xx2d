#pragma once
#include "pch.h"
#include "logic_base.h"

namespace xx {

	// reference from cocos 2.x AffineTransform
	struct AffineTransform {
		float a, b, c, d;
		float tx, ty;

		inline static AffineTransform Concat(AffineTransform const& t1, AffineTransform const& t2) {
			return { t1.a * t2.a + t1.b * t2.c, 
				t1.a * t2.b + t1.b * t2.d,
				t1.c * t2.a + t1.d * t2.c, 
				t1.c * t2.b + t1.d * t2.d,
				t1.tx * t2.a + t1.ty * t2.c + t2.tx,
				t1.tx * t2.b + t1.ty * t2.d + t2.ty };
		}

		inline static AffineTransform Translate(AffineTransform const& t, float tx, float ty) {
			return { t.a, t.b, t.c, t.d, t.tx + t.a * tx + t.c * ty, t.ty + t.b * tx + t.d * ty };
		}

		inline static AffineTransform CCAffineTransformScale(const AffineTransform& t, float sx, float sy) {
			return { t.a * sx, t.b * sx, t.c * sy, t.d * sy, t.tx, t.ty };
		}

		inline static AffineTransform CCAffineTransformRotate(const AffineTransform& t, float radians) {
			float fSin = sinf(radians);
			float fCos = cosf(radians);
			return { t.a * fCos + t.c * fSin,
				t.b * fCos + t.d * fSin,
				t.c * fCos - t.a * fSin,
				t.d * fCos - t.b * fSin,
				t.tx, t.ty };
		}

		XY Apply(XY const& point) const {
			return { (float)((double)a * point.x + (double)c * point.y + tx),
			(float)((double)b * point.x + (double)d * point.y + ty) };
		}
	};

	struct Node {
		AffineTransform transform;
		XY pos{}, anchor{}, anchorSize{}, size{}, scale{};
		float radians{};

		void FillTransform() {
			auto x = pos.x;
			auto y = pos.y;

			float cx = 1, sx = 0, cy = 1, sy = 0;

			if (radians) {
				auto c = cosf(-radians);
				auto s = sinf(-radians);
				cx = c * scale.x;
				cy = c * scale.y;
				sx = -s * scale.x;
				sy = s * scale.y;
			}

			if (!anchorSize.IsZero()) {
				x += cy * -anchorSize.x + sx * -anchorSize.y;
				y += sy * -anchorSize.x + cx * -anchorSize.y;
			}

			transform = { cy, sy, sx, cx, x, y };
		}
	};

}


struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

};
