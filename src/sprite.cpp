#include "pch.h"

namespace xx {

	bool Sprite::Empty() const {
		return !frame;
	}

	void Sprite::SetTexture(xx::Shared<GLTexture> t) {
		dirtyFrame = 1;
		auto&& f = *frame.Emplace();
		f.anchor = { 0.5, 0.5 };
		f.textureRotated = false;
		f.spriteSize = frame->spriteSourceSize = { (float)std::get<1>(t->vs), (float)std::get<2>(t->vs) };
		f.spriteOffset = { 0, 0 };
		f.textureRect = { 0, 0, frame->spriteSize };
		f.tex = std::move(t);
	}

	void Sprite::SetFrame(xx::Shared<Frame> f, bool overrideAnchor) {
		dirtyFrame = 1;
		frame = std::move(f);
		if (overrideAnchor && frame->anchor.has_value() && anchor != *frame->anchor) {
			dirtySizeAnchorPosScaleRotate = 1;
			anchor = *frame->anchor;
		}
	}

	void Sprite::SetFlipX(bool const& fx) {
		dirtyFrame = 1;
		flipX = fx;
	}
	void Sprite::SetFlipY(bool const& fy) {
		dirtyFrame = 1;
		flipY = fy;
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
	void Sprite::SetScale(float const& s) {
		dirtySizeAnchorPosScaleRotate = 1;
		scale = { s, s };
	}

	void Sprite::SetPosition(XY const& p) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos = p;
	}
	void Sprite::SetPositionX(float const& x) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.x = x;
	}
	void Sprite::SetPositionY(float const& y) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.y = y;
	}

	void Sprite::SetColor(RGBA8 const& c) {
		dirtyColor = 1;
		color = c;
	}

	void Sprite::Commit() {
		if (dirty) {
			if (dirtyFrame) {
				auto& r = frame->textureRect;
				if (frame->textureRotated) {
					if (flipX) {
						qv[0].v = r.y + r.w;
						qv[1].v = r.y + r.w;
						qv[2].v = r.y;
						qv[3].v = r.y;
					} else {
						qv[0].v = r.y;
						qv[1].v = r.y;
						qv[2].v = r.y + r.w;
						qv[3].v = r.y + r.w;
					}
					if (flipY) {
						qv[0].u = r.x + r.h;
						qv[1].u = r.x;
						qv[2].u = r.x;
						qv[3].u = r.x + r.h;
					} else {
						qv[0].u = r.x;
						qv[1].u = r.x + r.h;
						qv[2].u = r.x + r.h;
						qv[3].u = r.x;
					}
				} else {
					if (flipX) {
						qv[0].u = r.x + r.w;
						qv[1].u = r.x + r.w;
						qv[2].u = r.x;
						qv[3].u = r.x;
					} else {
						qv[0].u = r.x;
						qv[1].u = r.x;
						qv[2].u = r.x + r.w;
						qv[3].u = r.x + r.w;
					}
					if (flipY) {
						qv[0].v = r.y;
						qv[1].v = r.y + r.h;
						qv[2].v = r.y + r.h;
						qv[3].v = r.y;
					} else {
						qv[0].v = r.y + r.h;
						qv[1].v = r.y;
						qv[2].v = r.y;
						qv[3].v = r.y + r.h;
					}
				}
			}
			if (dirtySizeAnchorPosScaleRotate) {
				auto&& w = frame->spriteSize.w * scale.x;
				auto&& h = frame->spriteSize.h * scale.y;
				auto&& x = pos.x + frame->spriteOffset.x * scale.x - w * anchor.x;
				auto&& y = pos.y + frame->spriteOffset.y * scale.y - h * anchor.y;
				qv[0].x = x;				qv[0].y = y;
				qv[1].x = x;				qv[1].y = y + h;
				qv[2].x = x + w;			qv[2].y = y + h;
				qv[3].x = x + w;			qv[3].y = y;

				// todo: rotate support?
			}
			if (dirtyColor) {
				for (auto& v : qv) {
					memcpy(&v.r, &color, sizeof(color));
				}
			}
			dirty = 0;
		}
	}

	void Sprite::Draw(Engine* eg) {
		assert(!dirty);
		eg->sm.GetShader<Shader_XyUvC>().DrawQuad(*frame->tex, qv);
	}

	void Sprite::Draw(Engine* eg, Translate const& trans) {
		assert(!dirty);
		auto& s = eg->sm.GetShader<Shader_XyUvC>();
		auto&& q = s.DrawQuadBegin(*frame->tex);
		q[0].x = (qv[0].x + trans.offset.x) * trans.scale.x; q[0].y = (qv[0].y + trans.offset.y) * trans.scale.y;
		q[1].x = (qv[1].x + trans.offset.x) * trans.scale.x; q[1].y = (qv[1].y + trans.offset.y) * trans.scale.y;
		q[2].x = (qv[2].x + trans.offset.x) * trans.scale.x; q[2].y = (qv[2].y + trans.offset.y) * trans.scale.y;
		q[3].x = (qv[3].x + trans.offset.x) * trans.scale.x; q[3].y = (qv[3].y + trans.offset.y) * trans.scale.y;
		memcpy(&q[0].u, &qv[0].u, 8);	// 8: uv & color
		memcpy(&q[1].u, &qv[1].u, 8);
		memcpy(&q[2].u, &qv[2].u, 8);
		memcpy(&q[3].u, &qv[3].u, 8);
		s.DrawQuadEnd();
	}

}
