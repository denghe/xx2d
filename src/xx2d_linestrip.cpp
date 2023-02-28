#include "xx2d_pch.h"

namespace xx {

	std::vector<XY>& LineStrip::SetPoints() {
		dirty = true;
		return points;
	}

	LineStrip& LineStrip::FillCirclePoints(XY const& center, float const& radius, std::optional<float> const& angle, int const& segments, XY const& scale) {
		dirty = true;
		points.reserve(segments + 2);
		points.resize(segments + 1);
		auto coef = 2.0f * (float)M_PI / segments;
		auto a = angle.has_value() ? *angle : 0;
		for (int i = 0; i <= segments; ++i) {
			auto rads = i * coef + a;
			points[i].x = radius * cosf(rads) * scale.x + center.x;
			points[i].y = radius * sinf(rads) * scale.y + center.y;
		}
		if (angle.has_value()) {
			points.push_back(center);
		}
		return *this;
	}

	LineStrip& LineStrip::FillBoxPoints(XY const& center, XY const& wh) {
		auto hwh = wh / 2;
		points.resize(5);
		points[0] = { -hwh.x, -hwh.y };
		points[1] = { -hwh.x,hwh.y };
		points[2] = { hwh.x,hwh.y };
		points[3] = { hwh.x,-hwh.y };
		points[4] = { -hwh.x,-hwh.y };
		return *this;
	}

	LineStrip& LineStrip::SetSize(XY const& s) {
		dirty = true;
		size = s;
		return *this;
	}

	LineStrip& LineStrip::SetAnchor(XY const& a) {
		dirty = true;
		anchor = a;
		return *this;
	}

	LineStrip& LineStrip::SetRotate(float const& r) {
		dirty = true;
		radians = r;
		return *this;
	}

	LineStrip& LineStrip::SetScale(XY const& s) {
		dirty = true;
		scale = s;
		return *this;
	}
	LineStrip& LineStrip::SetScale(float const& s) {
		dirty = true;
		scale = { s, s };
		return *this;
	}

	LineStrip& LineStrip::SetPosition(XY const& p) {
		dirty = true;
		pos = p;
		return *this;
	}

	LineStrip& LineStrip::SetColor(RGBA8 const& c) {
		dirty = true;
		color = c;
		return *this;
	}

	void LineStrip::Commit() {
		if (dirty) {
			auto&& ps = points.size();
			pointsBuf.resize(ps);
			at = at.MakePosScaleRadiansAnchorSize(pos, scale, radians, size * anchor);
			for (size_t i = 0; i < ps; ++i) {
				(XY&)pointsBuf[i].x = at.Apply(points[i]);
				memcpy(&pointsBuf[i].r, &color, sizeof(color));
			}
			dirty = false;
		}
	}

	void LineStrip::Draw() {
		Commit();
		if (auto&& ps = pointsBuf.size()) {
			auto&& s = engine.sm.GetShader<Shader_LineStrip>();
			memcpy(s.DrawLineStrip(ps), pointsBuf.data(), ps * sizeof(XYRGBA8));
		}
	}

	void LineStrip::Draw(AffineTransform const& t) {
		Commit();
		if (auto&& ps = pointsBuf.size()) {
			auto&& s = engine.sm.GetShader<Shader_LineStrip>();
			auto&& buf = s.DrawLineStrip(ps);
			for (size_t i = 0; i < ps; ++i) {
				(XY&)buf[i].x = t.Apply(pointsBuf[i]);
				memcpy(&buf[i].r, &color.r, sizeof(color));
			}
		}
	}

}
