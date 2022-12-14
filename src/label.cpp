#include "pch.h"
#include "label.h"

void Label::SetText(BMFont& bmf, std::string_view const& txt, float const& fontSize, XY const& pos) {
	// todo: fontSize, scale, utf8, kerning?  xadvance??

	chars.clear();
	auto scale = fontSize / bmf.fontSize;
	lastPos = pos;
	float px = pos.x, py = pos.y;	// cursor pos
	for (auto& t : txt) {
		if (auto&& r = bmf.GetChar(t)) {
			auto&& c = chars.emplace_back();
			c.tex = bmf.texs[r->page];

			auto x = px + r->xoffset * scale / 2;
			auto y = py - r->yoffset * scale / 2;
			auto w = scale * r->width;
			auto h = scale * r->height;
			auto wd2 = w / 2;
			auto hd2 = h / 2;

			c.qv[0].x = x - wd2;  c.qv[0].y = y - hd2;
			c.qv[1].x = x - wd2;  c.qv[1].y = y + hd2;
			c.qv[2].x = x + wd2;  c.qv[2].y = y + hd2;
			c.qv[3].x = x + wd2;  c.qv[3].y = y - hd2;

			c.qv[0].u = r->x;               c.qv[0].v = r->y + r->height;
			c.qv[1].u = r->x;               c.qv[1].v = r->y;
			c.qv[2].u = r->x + r->width;    c.qv[2].v = r->y;
			c.qv[3].u = r->x + r->width;    c.qv[3].v = r->y + r->height;

			px += r->xadvance * scale;
		}
		else {
			px += bmf.fontSize;	
		}
		//y += bmf.lineHeight;
	}
	// todo: store px, py for calc entire width & height? bonding box?
	// todo: anchor fix? align mode?
	// todo: sort by tex
}


//void Label::SetScale(XY const& xy) {
//}
//
//
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
