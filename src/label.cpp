#include "pch.h"
#include "label.h"

void Label::SetText(BMFont& bmf, float const& fontSize, std::string_view const& txt) {
	chars.clear();

	XY tsizMscaleD2{ bmf.fontSize / 2, bmf.lineHeight / 2 };
	float x = 0, y = 0;
	for (auto& t : txt) {
		if (auto&& r = bmf.GetChar(t)) {			// todo: utf8 support
			auto&& c = chars.emplace_back();
			c.tex = bmf.texs[r->page];				// todo: xoffset, yoffset, xadvance, kerning handle
			c.qv[0].u = r->x;					c.qv[0].v = r->y + r->height;
			c.qv[1].u = r->x;					c.qv[1].v = r->y;
			c.qv[2].u = r->x + r->width;        c.qv[2].v = r->y;
			c.qv[3].u = r->x + r->width;        c.qv[3].v = r->y + r->height;

			c.qv[0].x = c.qv[1].x = x - tsizMscaleD2.x;
			c.qv[2].x = c.qv[3].x = x + tsizMscaleD2.x;
			c.qv[0].y = c.qv[3].y = y - tsizMscaleD2.y;
			c.qv[1].y = c.qv[2].y = y + tsizMscaleD2.y;
		}
		x += bmf.fontSize;	// todo: fontSize support
		//y += bmf.lineHeight;
	}
	
	//auto scale = bmf.fontSize / fontSize;
	//tsizMscaleD2.x = bmf.fontSize * scale / 2.f;
	//tsizMscaleD2.y = bmf.lineHeight * scale / 2.f;

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
