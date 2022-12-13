#include "pch.h"
#include "label.h"

void Label::SetText(BMFont& bmf, float const& fontSize, std::string_view const& txt) {
	//tex = bmf.tex;
	
	//tsizMscaleD2.x = TW() * xy.x / 2.f;
	//tsizMscaleD2.y = TH() * xy.y / 2.f;

	// todo

	//qv[0].u = 0;       qv[0].v = TH();
	//qv[1].u = 0;       qv[1].v = 0;
	//qv[2].u = TW();    qv[2].v = 0;
	//qv[3].u = TW();    qv[3].v = TH();
}


void Label::SetScale(XY const& xy) {
}


void Label::SetPositon(XY const& xy) {
	for (size_t n = qvs.size(), i = 0; i < n; ++i) {
		auto& qv = qvs[i];
		qv[0].x = qv[1].x = xy.x - tsizMscaleD2.x;
		qv[2].x = qv[3].x = xy.x + tsizMscaleD2.x;
		qv[0].y = qv[3].y = xy.y - tsizMscaleD2.y;
		qv[1].y = qv[2].y = xy.y + tsizMscaleD2.y;
	}
}


void Label::SetColor(RGBA8 const& c) {
	for (auto& qv : qvs) {
		memcpy(&qv[0].r, &c, sizeof(c));
		memcpy(&qv[1].r, &c, sizeof(c));
		memcpy(&qv[2].r, &c, sizeof(c));
		memcpy(&qv[3].r, &c, sizeof(c));
	}
}


void Label::Draw(Engine* eg) {
	for (auto& qv : qvs) {
		eg->AutoBatchDrawQuad(*tex, qv);
		// todo: call batch version
	}
}
