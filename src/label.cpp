#include "pch.h"
#include "label.h"

void Label::SetText(BMFont& bmf, std::string_view const& txt, float const& fontSize) {

	chars.clear();
	auto scale = fontSize / bmf.fontSize;
	float px = 0, py = 0;
	for (auto& t : txt) {
		if (auto&& r = bmf.GetChar(t)) {
			auto&& c = chars.emplace_back();
			c.tex = bmf.texs[r->page];

			auto w = scale * r->width;
			auto h = scale * r->height;

			auto x = px + r->xoffset * scale;
			auto y = py - r->yoffset * scale;

			c.qv[0].x = x;                  c.qv[0].y = y;
			c.qv[1].x = x;                  c.qv[1].y = y - h;
			c.qv[2].x = x + w;              c.qv[2].y = y - h;
			c.qv[3].x = x + w;              c.qv[3].y = y;

			c.qv[0].u = r->x;               c.qv[0].v = r->y;
			c.qv[1].u = r->x;               c.qv[1].v = r->y + r->height;
			c.qv[2].u = r->x + r->width;    c.qv[2].v = r->y + r->height;
			c.qv[3].u = r->x + r->width;    c.qv[3].v = r->y;

			px += r->xadvance * scale;
		}
		else {
			px += bmf.fontSize * scale;
		}
	}
	lastSize = { px, (float)bmf.lineHeight * scale };

	// todo: utf8, kerning?
	// todo: sort by tex
}


void Label::SetAnchor(XY const& a) {
	SetPositon({ lastPos.x - lastSize.x * a.x, lastPos.y + lastSize.y * a.y });
}


void Label::SetPositon(XY const& pos) {
	auto dx = pos.x - lastPos.x;
	auto dy = pos.y - lastPos.y;
	lastPos = pos;
	for(auto& o : chars) {
		for (auto& v : o.qv) {
			v.x += dx;
			v.y += dy;
		}
	}
}


void Label::SetColor(RGBA8 const& c) {
	for (auto& o : chars) {
		memcpy(&o.qv[0].r, &c, sizeof(c));
		memcpy(&o.qv[1].r, &c, sizeof(c));
		memcpy(&o.qv[2].r, &c, sizeof(c));
		memcpy(&o.qv[3].r, &c, sizeof(c));
	}
}


void Label::Draw(Engine* eg) {
	for (auto& c : chars) {
		eg->AutoBatchDrawQuad(*c.tex, c.qv);		// todo: batch version
	}
}
