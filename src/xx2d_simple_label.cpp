#include "xx2d_pch.h"

namespace xx {

	SimpleLabel& SimpleLabel::SetText(BMFont bmf, std::string_view const& text, float const& fontSize) {
		assert(bmf.texs.size() == 1);

		// todo: utf8, kerning?

		tex = bmf.texs[0];

		chars.clear();
		baseScale = fontSize / bmf.fontSize;
		float px = 0, py = 0;
		for (auto& t : text) {
			if (auto&& r = bmf.GetChar(t)) {
				auto&& c = chars.emplace_back();

				c.pos.x = px + r->xoffset * baseScale;
				c.pos.y = py - r->yoffset * baseScale;
				c.tx = r->x;
				c.ty = r->y;
				c.tw = r->width;
				c.th = r->height;

				px += r->xadvance * baseScale;
			} else {
				px += bmf.fontSize * baseScale;
			}
		}
		size = { px, (float)bmf.lineHeight * baseScale };
		return *this;
	}

	SimpleLabel& SimpleLabel::SetAnchor(XY const& a) {
		anchor = a;
		return *this;
	}

	SimpleLabel& SimpleLabel::SetScale(XY const& s) {
		scale = s;
		return *this;
	}
	SimpleLabel& SimpleLabel::SetScale(float const& s) {
		scale = { s, s };
		return *this;
	}

	SimpleLabel& SimpleLabel::SetPosition(XY const& p) {
		pos = p;
		return *this;
	}

	SimpleLabel& SimpleLabel::SetPositionX(float const& x) {
		pos.x = x;
		return *this;
	}
	SimpleLabel& SimpleLabel::SetPositionY(float const& y) {
		pos.y = y;
		return *this;
	}

	SimpleLabel& SimpleLabel::SetColor(RGBA8 const& c) {
		color = c;
		return *this;
	}

	void SimpleLabel::Draw() {
		auto siz = chars.size();
		auto qs = engine.sm.GetShader<Shader_QuadInstance>().Draw(*tex, siz);
		for (size_t i = 0; i < siz; i++) {
			auto& c = chars[i];
			auto& q = qs[i];
			q.anchor = {0,1};
			q.color = color;
			q.pos = size * xx::XY{ -anchor.x, 1 - anchor.y } + pos + c.pos;
			q.radians = 0;
			q.scale = scale * baseScale;
			q.texRectX = c.tx;
			q.texRectY = c.ty;
			q.texRectW = c.tw;
			q.texRectH = c.th;
		}
	}
}
