#include "xx2d_pch.h"

namespace xx {

	bool PolygonSprite::Empty() const {
		return !frame;
	}

	PolygonSprite& PolygonSprite::SetTexture(xx::Shared<GLTexture> t) {
		return SetFrame(MakeFrame(std::move(t)), false);
	}

	PolygonSprite& PolygonSprite::SetFrame(xx::Shared<Frame> f, bool overrideAnchor) {
		dirtyFrame = 1;
		frame = std::move(f);
		if (overrideAnchor && frame->anchor.has_value() && anchor != *frame->anchor) {
			dirtySizeAnchorPosScaleRotate = 1;
			anchor = *frame->anchor;
		}
		return *this;
	}

	XY& PolygonSprite::Size() const {
		assert(frame);
		return frame->spriteSize;
	}

	PolygonSprite& PolygonSprite::SetFlipX(bool const& fx) {
		dirtyFrame = 1;
		flip.x = fx ? -1 : 1;
		return *this;
	}
	PolygonSprite& PolygonSprite::SetFlipY(bool const& fy) {
		dirtyFrame = 1;
		flip.y = fy ? -1 : 1;
		return *this;
	}

	PolygonSprite& PolygonSprite::SetAnchor(XY const& a) {
		dirtySizeAnchorPosScaleRotate = 1;
		anchor = a;
		return *this;
	}

	PolygonSprite& PolygonSprite::SetRotate(float const& r) {
		dirtySizeAnchorPosScaleRotate = 1;
		radians = r;
		return *this;
	}

	PolygonSprite& PolygonSprite::AddRotate(float const& r) {
		dirtySizeAnchorPosScaleRotate = 1;
		radians += r;
		return *this;
	}

	PolygonSprite& PolygonSprite::SetScale(XY const& s) {
		dirtySizeAnchorPosScaleRotate = 1;
		scale = s;
		return *this;
	}
	PolygonSprite& PolygonSprite::SetScale(float const& s) {
		dirtySizeAnchorPosScaleRotate = 1;
		scale = { s, s };
		return *this;
	}

	PolygonSprite& PolygonSprite::SetPosition(XY const& p) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos = p;
		return *this;
	}
	PolygonSprite& PolygonSprite::SetPositionX(float const& x) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.x = x;
		return *this;
	}
	PolygonSprite& PolygonSprite::SetPositionY(float const& y) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.y = y;
		return *this;
	}

	PolygonSprite& PolygonSprite::AddPosition(XY const& p) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos += p;
		return *this;
	}
	PolygonSprite& PolygonSprite::AddPositionX(float const& x) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.x += x;
		return *this;
	}
	PolygonSprite& PolygonSprite::AddPositionY(float const& y) {
		dirtySizeAnchorPosScaleRotate = 1;
		pos.y += y;
		return *this;
	}

	PolygonSprite& PolygonSprite::SetColor(RGBA8 const& c) {
		dirtyColor = 1;
		color = c;
		return *this;
	}

	PolygonSprite& PolygonSprite::SetParentAffineTransform(AffineTransform* const& t) {
		dirtyParentAffineTransform = 1;
		pat = t;
		return *this;
	}

	void PolygonSprite::Commit() {
		if (dirty) {
			if (dirtyFrame) {
				if (frame->triangles.empty()) {
					vs.resize(4);
					auto& r = frame->textureRect;
					if (frame->textureRotated) {
						vs[0].u = r.x;
						vs[0].v = r.y;
						vs[1].u = r.x + r.wh.y;
						vs[1].v = r.y;
						vs[2].u = r.x + r.wh.y;
						vs[2].v = r.y + r.wh.x;
						vs[3].u = r.x;
						vs[3].v = r.y + r.wh.x;
					} else {
						vs[0].u = r.x;
						vs[0].v = r.y + r.wh.y;
						vs[1].u = r.x;
						vs[1].v = r.y;
						vs[2].u = r.x + r.wh.x;
						vs[2].v = r.y;
						vs[3].u = r.x + r.wh.x;
						vs[3].v = r.y + r.wh.y;
					}
					is.resize(6);
					is.insert(is.begin(), { 0,1,2,0,2,3 });
				} else {
					auto uvs = (xx::XY*)frame->verticesUV.data();
					auto siz = frame->verticesUV.size() / 2;
					vs.resize(siz);
					for (size_t i = 0; i < siz; i++) {
						vs[i].u = uvs[i].x;
						vs[i].v = uvs[i].y;
					}
					is = frame->triangles;
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
				if (frame->triangles.empty()) {
					(XY&)vs[0].x = { at.tx, at.ty };
					(XY&)vs[1].x = at.Apply({ 0, wh.y });
					(XY&)vs[2].x = at.Apply({ wh.x, wh.y });
					(XY&)vs[3].x = at.Apply({ wh.x, 0 });
				} else {
					auto xys = (xx::XY*)frame->vertices.data();
					auto siz = vs.size();
					for (size_t i = 0; i < siz; i++) {
						(XY&)vs[i].x = at.Apply(xys[i]);
					}
				}
			}
			if (dirtyColor) {
				for (auto& v : vs) {
					memcpy(&v.r, &color, sizeof(color));
				}
			}
			dirty = 0;
		}
	}

	void PolygonSprite::Draw() {
		Commit();
		auto [offset, pv, pi] = engine.sm.GetShader<xx::Shader_Verts>().Draw(*frame->tex, vs.size(), is.size());
		memcpy(pv, vs.data(), sizeof(typename decltype(vs)::value_type) * vs.size());
		for (size_t i = 0; i < is.size(); i++) {
			pi[i] = offset + is[i];
		}

	}

	void PolygonSprite::SubDraw() {
		assert(pat);
		dirtyParentAffineTransform = true;
		Draw();
	}

	void PolygonSprite::Draw(AffineTransform const& t) {
		Commit();
		auto [offset, pv, pi] = engine.sm.GetShader<xx::Shader_Verts>().Draw(*frame->tex, vs.size(), is.size());
		for (size_t i = 0; i < vs.size(); ++i) {
			(XY&)pv[i].x = t.Apply(vs[i]);
			memcpy(&pv[i].u, &vs[i].u, 8);	// 8: uv & color
		}
		for (size_t i = 0; i < is.size(); i++) {
			pi[i] = offset + is[i];
		}
	}
}
