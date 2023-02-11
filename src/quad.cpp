#include "pch.h"

namespace xx {
	Quad& Quad::SetTexture(Shared<GLTexture> const& t) {
		tex = t;
		texRectX = 0;
		texRectY = 0;
		texRectW = (float)std::get<1>(t->vs);
		texRectH = (float)std::get<2>(t->vs);
		return *this;
	}

	Quad& Quad::SetFrame(Shared<Frame> const& f) {
		assert(f && !f->textureRotated);
		tex = f->tex;
		texRectX = f->textureRect.x;
		texRectY = f->textureRect.y;
		texRectW = f->textureRect.wh.x;
		texRectH = f->textureRect.wh.y;
		return *this;
	}

	void Quad::Draw() const {
		engine.sm.GetShader<Shader_QuadInstance>().DrawQuad(*tex, (QuadInstanceData*)this);
	}
}
