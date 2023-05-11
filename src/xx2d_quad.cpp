#include "xx2d.h"

namespace xx {
	Quad& Quad::SetTexture(Shared<GLTexture> const& t) {
		tex = t;
		texRectX = 0;
		texRectY = 0;
		texRectW = t->Width();
		texRectH = t->Height();
		return *this;
	}

	Quad& Quad::SetFrame(Shared<Frame> const& f) {
		assert(f && !f->textureRotated);
		tex = f->tex;
		texRectX = f->textureRect.x;
		texRectY = f->textureRect.y;
		texRectW = (uint16_t)f->textureRect.wh.x;
		texRectH = (uint16_t)f->textureRect.wh.y;
		return *this;
	}

	XY Quad::Size() const {
		assert(tex);
		return { (float)texRectW, (float)texRectH };
	}

	Quad& Quad::SetAnchor(XY const& a) {
		anchor = a;
		return *this;
	}

	Quad& Quad::SetRotate(float const& r) {
		radians = r;
		return *this;
	}

	Quad& Quad::AddRotate(float const& r) {
		radians += r;
		return *this;
	}

	Quad& Quad::SetScale(XY const& s) {
		scale = s;
		return *this;
	}
	Quad& Quad::SetScale(float const& s) {
		scale = { s, s };
		return *this;
	}

	Quad& Quad::SetPosition(XY const& p) {
		pos = p;
		return *this;
	}
	Quad& Quad::SetPositionX(float const& x) {
		pos.x = x;
		return *this;
	}
	Quad& Quad::SetPositionY(float const& y) {
		pos.y = y;
		return *this;
	}

	Quad& Quad::AddPosition(XY const& p) {
		pos += p;
		return *this;
	}
	Quad& Quad::AddPositionX(float const& x) {
		pos.x += x;
		return *this;
	}
	Quad& Quad::AddPositionY(float const& y) {
		pos.y += y;
		return *this;
	}

	Quad& Quad::SetColor(RGBA8 const& c) {
		color = c;
		return *this;
	}
	Quad& Quad::SetColorA(uint8_t const& a) {
		color.a = a;
		return *this;

	}
	Quad& Quad::SetColorAf(float const& a) {
		color.a = 255 * a;
		return *this;
	}

	void Quad::Draw() const {
		engine.sm.GetShader<Shader_QuadInstance>().Draw(*tex, (QuadInstanceData*)this);
	}
}
