#pragma once
#include "pch.h"

namespace xx {

	AffineTransform AffineTransform::Make(XY const& pos, XY const& anchorSize, XY const& scale, float const& radians) {
		auto x = pos.x;
		auto y = pos.y;
		float cx = 1, sx = 0, cy = 1, sy = 0;
		if (radians) {
			auto c = std::cos(-radians);
			auto s = std::sin(-radians);
			cx = c * scale.x;
			cy = c * scale.y;
			sx = -s * scale.x;
			sy = s * scale.y;
		}
		if (!anchorSize.IsZero()) {
			x += cy * -anchorSize.x + sx * -anchorSize.y;
			y += sy * -anchorSize.x + cx * -anchorSize.y;
		}
		return { cy, sy, sx, cx, x, y };
	}

	AffineTransform AffineTransform::MakeIdentity() {
		return { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };
	}

	AffineTransform AffineTransform::MakeConcat(AffineTransform const& t) const {
		return { a * t.a + b * t.c,
			a * t.b + b * t.d,
			c * t.a + d * t.c,
			c * t.b + d * t.d,
			tx * t.a + ty * t.c + t.tx,
			tx * t.b + ty * t.d + t.ty };
	}

	AffineTransform AffineTransform::MakeInvert(AffineTransform const& t) {
		float de = 1 / (t.a * t.d - t.b * t.c);
		return { de * t.d, -de * t.b, -de * t.c, de * t.a,
			de * (t.c * t.ty - t.d * t.tx), de * (t.b * t.tx - t.a * t.ty) };
	}

	AffineTransform& AffineTransform::Translate(XY const& pos) {
		tx += a * pos.x + c * pos.y;
		ty += b * pos.x + d * pos.y;
		return *this;
	}

	AffineTransform& AffineTransform::Scale(XY const& scale) {
		a *= scale.x;
		b *= scale.x;
		c *= scale.y;
		d *= scale.y;
		return *this;
	}

	AffineTransform& AffineTransform::Rotate(float const& radians) {
		auto fSin = std::sin(radians);
		auto fCos = std::cos(radians);
		auto ta = a * fCos + c * fSin;
		auto tb = b * fCos + d * fSin;
		auto tc = c * fCos - a * fSin;
		auto td = d * fCos - b * fSin;
		a = ta;
		b = tb;
		c = tc;
		d = td;
		return *this;
	}

	XY AffineTransform::Apply(XY const& point) const {
		return { (float)((double)a * point.x + (double)c * point.y + tx),
		(float)((double)b * point.x + (double)d * point.y + ty) };
	}
}
