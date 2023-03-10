#include "xx2d_pch.h"

namespace xx {

	Label& Label::SetText(BMFont bmf, std::string_view const& text, float const& fontSize, float const& lineWidthLimit) {
		dirty = 0xFFFFFFFFu;

		// todo: kerning? sort by tex?

		auto c32s = xx::StringU8ToU32(text);

		chars.clear();
		auto baseScale = fontSize / bmf.fontSize;
		float px{}, py{}, maxpx{}, lineHeight = bmf.lineHeight * baseScale;
		for (auto& t : c32s) {
			if (t == '\r') continue;
			else if (t == '\n') {
				px = 0;
				py -= lineHeight;
			} else if (auto&& r = bmf.GetChar(t)) {
				auto cw = r->xadvance * baseScale;
				if (lineWidthLimit > 0) {
					if (px + cw > lineWidthLimit) {
						if (px > maxpx) {
							maxpx = px;
						}
						px = 0;
						py -= lineHeight;
					}
				}
				auto&& c = chars.emplace_back();
				c.tex = bmf.texs[r->page];

				auto w = baseScale * r->width;
				auto h = baseScale * r->height;

				auto x = px + r->xoffset * baseScale;
				auto y = py - r->yoffset * baseScale;

				c.qv[0].x = x;                  c.qv[0].y = y;
				c.qv[1].x = x;                  c.qv[1].y = y - h;
				c.qv[2].x = x + w;              c.qv[2].y = y - h;
				c.qv[3].x = x + w;              c.qv[3].y = y;

				c.qv[0].u = r->x;               c.qv[0].v = r->y;
				c.qv[1].u = r->x;               c.qv[1].v = r->y + r->height;
				c.qv[2].u = r->x + r->width;    c.qv[2].v = r->y + r->height;
				c.qv[3].u = r->x + r->width;    c.qv[3].v = r->y;

				px += cw;

				// pos 0,0  anchor 0,0  scale 1,1 data store
				c.posBak[0] = c.qv[0];
				c.posBak[1] = c.qv[1];
				c.posBak[2] = c.qv[2];
				c.posBak[3] = c.qv[3];
			} else {
				px += bmf.fontSize * baseScale;
			}
		}
		size = { px, (float)bmf.lineHeight * baseScale };
		return *this;
	}

	Label& Label::SetAnchor(XY const& a) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		anchor = a;
		return *this;
	}

	Label& Label::SetRotate(float const& r) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		radians = r;
		return *this;
	}

	Label& Label::SetScale(XY const& s) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		scale = s;
		return *this;
	}
	Label& Label::SetScale(float const& s) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		scale = { s, s };
		return *this;
	}

	Label& Label::SetPosition(XY const& p) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		pos = p;
		return *this;
	}

	Label& Label::SetPositionX(float const& x) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		pos.x = x;
		return *this;
	}
	Label& Label::SetPositionY(float const& y) {
		dirtyTextSizeAnchorPosScaleRotate = 1;
		pos.y = y;
		return *this;
	}

	Label& Label::SetColor(RGBA8 const& c) {
		dirtyColor = 1;
		color = c;
		return *this;
	}

	void Label::Commit() {
		if (dirty) {
			if (dirtyTextSizeAnchorPosScaleRotate) {
				at = at.MakePosScaleRadiansAnchorSize(pos, scale, radians, { size.x * anchor.x, -size.y * (1-anchor.y) });
				for (auto& c : chars) {
					auto& qv = c.qv;
					(XY&)qv[0].x = at.Apply(c.posBak[0]);
					(XY&)qv[1].x = at.Apply(c.posBak[1]);
					(XY&)qv[2].x = at.Apply(c.posBak[2]);
					(XY&)qv[3].x = at.Apply(c.posBak[3]);
				}
			}
			if (dirtyColor) {
				for (auto& c : chars) {
					memcpy(&c.qv[0].r, &color, sizeof(color));
					memcpy(&c.qv[1].r, &color, sizeof(color));
					memcpy(&c.qv[2].r, &color, sizeof(color));
					memcpy(&c.qv[3].r, &color, sizeof(color));
				}
			}
			dirty = 0;
		}
	}
	 
	void Label::Draw() {
		Commit();
		auto& s = engine.sm.GetShader<Shader_Quad>();
		for (auto& c : chars) {
			s.Draw(*c.tex, c.qv);
		}
	}

	void Label::Draw(AffineTransform const& t) {
		Commit();
		auto& s = engine.sm.GetShader<Shader_Quad>();
		for (auto& c : chars) {
			auto&& q = *s.Draw(*c.tex);
			(XY&)q[0].x = t.Apply(c.qv[0]);
			memcpy(&q[0].u, &c.qv[0].u, 8);	// 8: uv & color
			(XY&)q[1].x = t.Apply(c.qv[1]);
			memcpy(&q[1].u, &c.qv[1].u, 8);
			(XY&)q[2].x = t.Apply(c.qv[2]);
			memcpy(&q[2].u, &c.qv[2].u, 8);
			(XY&)q[3].x = t.Apply(c.qv[3]);
			memcpy(&q[3].u, &c.qv[3].u, 8);
		}
	}
}
