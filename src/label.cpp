#include "pch.h"
#include "label.h"

void Label::SetText(BMFont bmf, std::string_view const& text, float const& fontSize) {
	dirty = 0xFFFFFFFFu;

	// todo: utf8, kerning?
	// todo: sort by tex

	chars.clear();
	auto s = fontSize / bmf.fontSize;
	float px = 0, py = 0;
	for (auto& t : text) {
		if (auto&& r = bmf.GetChar(t)) {
			auto&& c = chars.emplace_back();
			c.tex = bmf.texs[r->page];

			auto w = s * r->width;
			auto h = s * r->height;

			auto x = px + r->xoffset * s;
			auto y = py - r->yoffset * s;

			c.qv[0].x = x;                  c.qv[0].y = y;
			c.qv[1].x = x;                  c.qv[1].y = y - h;
			c.qv[2].x = x + w;              c.qv[2].y = y - h;
			c.qv[3].x = x + w;              c.qv[3].y = y;

			c.qv[0].u = r->x;               c.qv[0].v = r->y;
			c.qv[1].u = r->x;               c.qv[1].v = r->y + r->height;
			c.qv[2].u = r->x + r->width;    c.qv[2].v = r->y + r->height;
			c.qv[3].u = r->x + r->width;    c.qv[3].v = r->y;

			px += r->xadvance * s;

			// pos 0,0  anchor 0,0  scale 1,1 data store
			c.posBak[0] = c.qv[0];
			c.posBak[1] = c.qv[1];
			c.posBak[2] = c.qv[2];
			c.posBak[3] = c.qv[3];
		} else {
			px += bmf.fontSize * s;
		}
	}
	maxSize = { px, (float)bmf.lineHeight * s };
}

void Label::SetAnchor(XY const& a) {
	dirtyTextSizeAnchorPosScaleRotate = 1;
	anchor = a;
}

void Label::SetRotate(float const& r) {
	dirtyTextSizeAnchorPosScaleRotate = 1;
	rotate = r;
}

void Label::SetScale(XY const& s) {
	dirtyTextSizeAnchorPosScaleRotate = 1;
	scale = s;
}
void Label::SetScale(float const& s) {
	dirtyTextSizeAnchorPosScaleRotate = 1;
	scale = { s, s };
}

void Label::SetPositon(XY const& p) {
	dirtyTextSizeAnchorPosScaleRotate = 1;
	pos = p;
}

void Label::SetColor(RGBA8 const& c) {
	dirtyColor = 1;
	color = c;
}

void Label::Commit() {
	if (dirty) {
		if (dirtyTextSizeAnchorPosScaleRotate) {
			auto x = pos.x - maxSize.w * scale.x * anchor.x;
			auto y = pos.y + maxSize.h * scale.y * ( 1 - anchor.y );
			for (auto& o : chars) {
				for (size_t i = 0; i < o.qv.size(); ++i) {
					o.qv[i].x = o.posBak[i].x * scale.x + x;
					o.qv[i].y = o.posBak[i].y * scale.y + y;
				}
			}
			// todo: rotate support?
		}
		if (dirtyColor) {
			for (auto& o : chars) {
				for (auto& v : o.qv) {
					memcpy(&v.r, &color, sizeof(color));
				}
			}
		}
		dirty = 0;
	}
}

void Label::Draw(Engine* eg) {
	auto& s = eg->sm.GetShader<Shader_XyUvC>();
	for (auto& c : chars) {
		s.DrawQuad(*c.tex, c.qv);		// todo: batch version
	}
}
