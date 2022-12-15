#include "pch.h"

void Sprite::SetTexture(xx::Shared<GLTexture> t) {
	if (!frame) {
		frame.Emplace();
	}
	if (frame->tex == t) return;
	dirtyFrame = 1;
	frame->tex = std::move(t);
	// todo: fill frame members
}

void Sprite::SetTexture(xx::Shared<Frame> f) {
	frame = std::move(f);
	dirtyFrame = 1;
}

void Sprite::SetScale(XY const& s) {
}

void Sprite::SetPositon(XY const& p) {
}

void Sprite::SetColor(RGBA8 const& c) {
}

void Sprite::Draw(Engine* eg) {
	//if (dirty) {
	//	// todo: fill qv by texRect, pos + siz + scale + rotate

	//	auto&& w = std::get<1>(tex->vs);
	//	auto&& h = std::get<2>(tex->vs);

	//	qv[0].u = 0;		qv[0].v = h;
	//	qv[1].u = 0;		qv[1].v = 0;
	//	qv[2].u = w;		qv[2].v = 0;
	//	qv[3].u = h;		qv[3].v = h;

	//	auto dx = w * scale.x / 2.f;
	//	auto dy = h * scale.y / 2.f;

	//	qv[0].x = qv[1].x = pos.x - dx;
	//	qv[2].x = qv[3].x = pos.x + dx;
	//	qv[0].y = qv[3].y = pos.y - dy;
	//	qv[1].y = qv[2].y = pos.y + dy;

	//	memcpy(&qv[0].r, &color, sizeof(color));
	//	memcpy(&qv[1].r, &color, sizeof(color));
	//	memcpy(&qv[2].r, &color, sizeof(color));
	//	memcpy(&qv[3].r, &color, sizeof(color));
	//}

	//eg->AutoBatchDrawQuad(*tex, qv);
}
