#include "pch.h"
#include "logic.h"
#include "logic10.h"

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

void Logic10::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic10 Init( quad instance tests )" << std::endl;

	auto tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
	qs.resize(1000000);
	for (auto& q : qs) {
		q.SetTexture( tex );
		q.pos = { (float)rnd.Next(- xx::engine.w/2, xx::engine.w / 2), (float)rnd.Next(-xx::engine.h / 2, xx::engine.h / 2) };
		q.scale = 1;
		q.radians = 0;
		q.color = {255,255,255,255};
	}
}

int Logic10::Update() {
	for (auto& q : qs) {
		q.radians += 0.01;
		q.Draw();
	}
	return 0;
}
