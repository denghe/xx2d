#include "xx2d.h"

namespace xx {

	bool Sprite::Empty() const {
		return !frame;
	}

	Sprite& Sprite::SetTexture(xx::Shared<GLTexture> t) {
		return SetFrame(MakeFrame(std::move(t)), false);
	}

	Sprite& Sprite::SetFrame(xx::Shared<Frame> f, bool overrideAnchor) {
		dirtyFrame = 1;
		frame = std::move(f);
		if (overrideAnchor && frame->anchor.has_value() && anchor != *frame->anchor) {
			dirtySizeAnchorPosScaleRotate = 1;
			anchor = *frame->anchor;
		}
		return *this;
	}

	XY& Sprite::Size() const {
		assert(frame);
		return frame->spriteSize;
	}

	Sprite& Sprite::SetFlipX(bool const& fx) {
		dirtyFrame = 1;
		flip.x = fx ? -1 : 1;
		return *this;
	}
	Sprite& Sprite::SetFlipY(bool const& fy) {
		dirtyFrame = 1;
		flip.y = fy ? -1 : 1;
		return *this;
	}

	Sprite& Sprite::SetAnchor(XY const& a) {
		dirtySizeAnchorPosScaleRotate = 1;
		anchor = a;
		return *this;
	}

	Sprite& Sprite::SetRotate(float const& r) {
		dirtySizeAnchorPosScaleRotate = 1;
		radians = r;
		return *this;
	}

	Sprite& Sprite::AddRotate(float const& r) {
		dirtySizeAnchorPosScaleRotate = 1;
		radians += r;
		return *this;
	}

	Sprite& Sprite::SetScale(XY const& s) {
		dirtySizeAnchorPosScaleRotate = 1;
		scale = s;
		return *this;
	}
	Sprite& Sprite::SetScale(float const& s) {
		dirtySizeAnchorPosScaleRotate = 1;
		scale = { s, s };
		return *this;
	}

	Sprite& Sprite::SetPosition(XY const& p) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos = p;
		return *this;
	}
	Sprite& Sprite::SetPositionX(float const& x) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.x = x;
		return *this;
	}
	Sprite& Sprite::SetPositionY(float const& y) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.y = y;
		return *this;
	}

	Sprite& Sprite::AddPosition(XY const& p) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos += p;
		return *this;
	}
	Sprite& Sprite::AddPositionX(float const& x) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.x += x;
		return *this;
	}
	Sprite& Sprite::AddPositionY(float const& y) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.y += y;
		return *this;
	}

	Sprite& Sprite::SetColor(RGBA8 const& c) {
		dirtyColor = 1;
		color = c;
		return *this;
	}

	Sprite& Sprite::SetParentAffineTransform(AffineTransform* const& t) {
		dirtyParentAffineTransform = 1;
		pat = t;
		return *this;
	}

	void Sprite::Commit() {
		if (dirty) {
			if (dirtyFrame) {
				auto& r = frame->textureRect;
				if (frame->textureRotated) {
						qv[0].v = r.y;
						qv[1].v = r.y;
						qv[2].v = r.y + r.wh.x;
						qv[3].v = r.y + r.wh.x;
						qv[0].u = r.x;
						qv[1].u = r.x + r.wh.y;
						qv[2].u = r.x + r.wh.y;
						qv[3].u = r.x;
				} else {
						qv[0].u = r.x;
						qv[1].u = r.x;
						qv[2].u = r.x + r.wh.x;
						qv[3].u = r.x + r.wh.x;
						qv[0].v = r.y + r.wh.y;
						qv[1].v = r.y;
						qv[2].v = r.y;
						qv[3].v = r.y + r.wh.y;
				}
			}
			if (dirtySizeAnchorPosScaleRotate || dirtyParentAffineTransform) {
				auto wh = frame->spriteSize;
				if (dirtySizeAnchorPosScaleRotate) {
					at = at.MakePosScaleRadiansAnchorSize(pos, scale * flip, radians, wh * anchor);
					atBak = at;
				}
				if (dirtyParentAffineTransform) {
					if (pat) {
						at = atBak.MakeConcat(*pat);
					}
					else {
						at = atBak;
					}
				}
				(XY&)qv[0].x = { at.tx, at.ty };
				(XY&)qv[1].x = at.Apply({0, wh.y });
				(XY&)qv[2].x = at.Apply({wh.x, wh.y});
				(XY&)qv[3].x = at.Apply({wh.x, 0});
			}
			if (dirtyColor) {
				for (auto& v : qv) {
					memcpy(&v.r, &color, sizeof(color));
				}
			}
			dirty = 0;
		}
	}

	void Sprite::Draw() {
		Commit();
		engine.sm.GetShader<Shader_Quad>().Draw(*frame->tex, qv);
	}

	void Sprite::SubDraw() {
		assert(pat);
		dirtyParentAffineTransform = true;
		Draw();
	}

	void Sprite::Draw(AffineTransform const& t) {
		Commit();
		auto&& q = *engine.sm.GetShader<Shader_Quad>().Draw(*frame->tex);
		(XY&)q[0].x = t.Apply(qv[0]);
		memcpy(&q[0].u, &qv[0].u, 8);	// 8: uv & color
		(XY&)q[1].x = t.Apply(qv[1]);
		memcpy(&q[1].u, &qv[1].u, 8);
		(XY&)q[2].x = t.Apply(qv[2]);
		memcpy(&q[2].u, &qv[2].u, 8);
		(XY&)q[3].x = t.Apply(qv[3]);
		memcpy(&q[3].u, &qv[3].u, 8);
	}
}
