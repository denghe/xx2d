#include "pch.h"
#include "label.h"

void Label::SetText(BMFont& bmf, float const& fontSize, std::string_view const& txt) {
	chars.clear();
	// todo: fontSize support, scale, utf8, xoffset, yoffset, xadvance, kerning handle
	auto sx = bmf.fontSize / 2;
	auto sy = bmf.lineHeight / 2;
	float x = 0, y = 0;
	for (auto& t : txt) {
		if (auto&& r = bmf.GetChar(t)) {
			auto&& c = chars.emplace_back();
			c.tex = bmf.texs[r->page];

			c.qv[0].x = x - sx;				c.qv[0].y = y - sy;
			c.qv[0].u = r->x;				c.qv[0].v = r->y + r->height;
			c.qv[1].x = x - sx;				c.qv[1].y = y + sy;
			c.qv[1].u = r->x;				c.qv[1].v = r->y;
			c.qv[2].x = x + sx;				c.qv[2].y = y + sy;
			c.qv[2].u = r->x + r->width;    c.qv[2].v = r->y;
			c.qv[3].x = x + sx;				c.qv[3].y = y - sy;
			c.qv[3].u = r->x + r->width;    c.qv[3].v = r->y + r->height;
		}
		x += bmf.fontSize;	
		//y += bmf.lineHeight;
	}

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
