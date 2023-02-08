#include "pch.h"

namespace xx {

	std::vector<XY>& LineStrip::SetPoints() {
		dirty = true;
		return points;
	}

	void LineStrip::FillCirclePoints(XY const& center, float const& radius, std::optional<float> const& angle, int const& segments, XY const& scale) {
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
	}

	void LineStrip::FillBoxPoints(XY const& center, XY const& wh) {
		auto hwh = wh / 2;
		points.resize(5);
		points[0] = { -hwh.x, -hwh.y };
		points[1] = { -hwh.x,hwh.y };
		points[2] = { hwh.x,hwh.y };
		points[3] = { hwh.x,-hwh.y };
		points[4] = { -hwh.x,-hwh.y };
	}

	void LineStrip::SetSize(Size const& s) {
		dirty = true;
		size = s;
	}

	void LineStrip::SetAnchor(XY const& a) {
		dirty = true;
		anchor = a;
	}

	void LineStrip::SetRotate(float const& r) {
		dirty = true;
		rotate = r;
	}

	void LineStrip::SetScale(XY const& s) {
		dirty = true;
		scale = s;
	}
	void LineStrip::SetScale(float const& s) {
		dirty = true;
		scale = { s, s };
	}

	void LineStrip::SetPositon(XY const& p) {
		dirty = true;
		pos = p;
	}

	void LineStrip::SetColor(RGBA8 const& c) {
		dirty = true;
		color = c;
	}

	void LineStrip::Commit() {
		if (dirty) {
			auto&& ps = points.size();
			pointsBuf.resize(ps);
			auto x = pos.x - size.w * scale.x * anchor.x;
			auto y = pos.y - size.h * scale.y * anchor.y;
			for (size_t i = 0; i < ps; ++i) {
				pointsBuf[i].x = points[i].x * scale.x + x;
				pointsBuf[i].y = points[i].y * scale.y + y;
				memcpy(&pointsBuf[i].r, &color, sizeof(color));
			}
			// todo: rotate support?
			dirty = false;
		}
	}

	void LineStrip::Draw(Engine* eg) {
		assert(!dirty);
		if (auto&& ps = pointsBuf.size()) {
			auto&& s = eg->sm.GetShader<Shader_XyC>();
			memcpy(s.DrawLineStrip(ps), pointsBuf.data(), ps * sizeof(XYRGBA8));
		}
	}

	void LineStrip::Draw(Engine* eg, Translate const& trans) {
		assert(!dirty);
		if (auto&& ps = pointsBuf.size()) {
			auto&& s = eg->sm.GetShader<Shader_XyC>();
			auto&& buf = s.DrawLineStrip(ps);
			for (size_t i = 0; i < ps; ++i) {
				buf[i].x = (pointsBuf[i].x + trans.offset.x) * trans.scale.x;
				buf[i].y = (pointsBuf[i].y + trans.offset.y) * trans.scale.y;
				memcpy(&buf[i].r, &color.r, sizeof(color));
			}
		}
	}

}
