#include "pch.h"

GLsizei& Sprite::TW() const {
	return std::get<1>(tex->vs);
}

GLsizei& Sprite::TH() const {
	return std::get<2>(tex->vs);
}

void Sprite::SetTexture(xx::Shared<GLTexture> t_) {
	tex = std::move(t_);

	qv[0].u = 0;       qv[0].v = TH();
	qv[1].u = 0;       qv[1].v = 0;
	qv[2].u = TW();    qv[2].v = 0;
	qv[3].u = TW();    qv[3].v = TH();

	//qv[0].i = 0;
	//qv[1].i = 0;
	//qv[2].i = 0;
	//qv[3].i = 0;

	// todo: set uv & texture index
}

void Sprite::SetScale(XY const& xy) {
	tsizMscaleD2.x = TW() * xy.x / 2.f;
	tsizMscaleD2.y = TH() * xy.y / 2.f;
}

void Sprite::SetPositon(XY const& xy) {
	qv[0].x = qv[1].x = xy.x - tsizMscaleD2.x;
	qv[2].x = qv[3].x = xy.x + tsizMscaleD2.x;
	qv[0].y = qv[3].y = xy.y - tsizMscaleD2.y;
	qv[1].y = qv[2].y = xy.y + tsizMscaleD2.y;
}

void Sprite::SetColor(RGBA8 const& c) {
	memcpy(&qv[0].r, &c, sizeof(c));
	memcpy(&qv[1].r, &c, sizeof(c));
	memcpy(&qv[2].r, &c, sizeof(c));
	memcpy(&qv[3].r, &c, sizeof(c));
}

void Sprite::Draw(Engine* eg) {
	eg->AutoBatchDrawQuad(*tex, qv);
}
