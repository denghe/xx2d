#include "pch.h"

void Sprite::SetTexture(xx::Shared<GLTexture> t) {
	if (!frame) {
		frame.Emplace();
	}
	if (frame->tex == t) return;
	dirtyFrame = 1;
	frame->textureRotated = false;
	frame->spriteSize = { (float)std::get<1>(t->vs), (float)std::get<2>(t->vs) };
	frame->spriteSourceSize = frame->spriteSize;
	frame->spriteOffset = { frame->spriteSize.w * anchor.x, frame->spriteSize.h * anchor.y };
	frame->textureRect = { 0, 0, frame->spriteSize };
	frame->tex = std::move(t);
}

void Sprite::SetTexture(xx::Shared<Frame> f) {
	frame = std::move(f);
	dirtyFrame = 1;
	dirtySizeAnchorPosScaleRotate = 1;
	anchor = { frame->spriteOffset.x / frame->spriteSize.w, frame->spriteOffset.y / frame->spriteSize.h };
}

void Sprite::SetAnchor(XY const& a) {
	dirtySizeAnchorPosScaleRotate = 1;
	anchor = a;
}

void Sprite::SetRotate(float const& r) {
	dirtySizeAnchorPosScaleRotate = 1;
	rotate = r;
}

void Sprite::SetScale(XY const& s) {
	dirtySizeAnchorPosScaleRotate = 1;
	scale = s;
}

void Sprite::SetPositon(XY const& p) {
	dirtySizeAnchorPosScaleRotate = 1;
	pos = p;
}

void Sprite::SetColor(RGBA8 const& c) {
	dirtyColor = 1;
	color = c;
}

void Sprite::Draw(Engine* eg) {
	if (dirty) {
		if (dirtyFrame) {
			auto& r = frame->textureRect;
			qv[0].u = r.x;              qv[0].v = r.y + r.h;
			qv[1].u = r.x;              qv[1].v = r.y;
			qv[2].u = r.x + r.w;        qv[2].v = r.y;
			qv[3].u = r.x + r.w;        qv[3].v = r.y + r.h;
		}
		if (dirtySizeAnchorPosScaleRotate) {
			auto dx = frame->spriteSize.w * scale.x / 2.f;
			auto dy = frame->spriteSize.h * scale.y / 2.f;

			// todo
			// frame->spriteSourceSize
			// rotate

			qv[0].x = qv[1].x = pos.x + frame->spriteSize.w * anchor.x - dx;
			qv[2].x = qv[3].x = pos.x + frame->spriteSize.w * anchor.x + dx;
			qv[0].y = qv[3].y = pos.y + frame->spriteSize.h * anchor.y - dy;
			qv[1].y = qv[2].y = pos.y + frame->spriteSize.h * anchor.y + dy;
		}
		if (dirtyColor) {
			memcpy(&qv[0].r, &color, sizeof(color));
			memcpy(&qv[1].r, &color, sizeof(color));
			memcpy(&qv[2].r, &color, sizeof(color));
			memcpy(&qv[3].r, &color, sizeof(color));
		}
		dirty = 0;
	}
	eg->AutoBatchDrawQuad(*frame->tex, qv);
}
