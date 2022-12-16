#include "pch.h"

void Sprite::SetTexture(xx::Shared<GLTexture> t) {
	if (!frame) {
		frame.Emplace();
	}
	if (frame->tex == t) return;
	dirtyFrame = 1;
	frame->anchor = { 0.5, 0.5 };
	frame->textureRotated = false;
	frame->spriteSize = frame->spriteSourceSize = { (float)std::get<1>(t->vs), (float)std::get<2>(t->vs) };
	frame->spriteOffset = { 0, 0 };
	frame->textureRect = { 0, 0, frame->spriteSize };
	frame->tex = std::move(t);
}

void Sprite::SetTexture(xx::Shared<Frame> f) {
	dirtyFrame = 1;
	frame = std::move(f);
	if (frame->anchor.has_value() && anchor != *frame->anchor) {
		dirtySizeAnchorPosScaleRotate = 1;
		anchor = *frame->anchor;
	}
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
			auto w = frame->spriteSize.w * scale.x;
			auto h = frame->spriteSize.h * scale.y;
			auto x = pos.x + frame->spriteOffset.x * scale.x - w * anchor.x;
			auto y = pos.y + frame->spriteOffset.y * scale.y + h * anchor.y;
			qv[0].x = x + w;				qv[0].y = y;
			qv[1].x = x + w;				qv[1].y = y + h;
			qv[2].x = x;					qv[2].y = y + h;
			qv[3].x = x;					qv[3].y = y;

			// todo: rotate support?
		}
		if (dirtyColor) {
			for (auto& v : qv) {
				memcpy(&v.r, &color, sizeof(color));
			}
		}
		dirty = 0;
	}
	eg->AutoBatchDrawQuad(*frame->tex, qv);
}
