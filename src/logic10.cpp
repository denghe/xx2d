#include "pch.h"
#include "logic.h"
#include "logic10.h"

namespace xx {
	Quad& Quad::SetTexture(Shared<GLTexture> t) {
		auto&& f = *frame.Emplace();
		f.anchor = { 0.5, 0.5 };
		f.textureRotated = false;
		f.spriteSize = frame->spriteSourceSize = { (float)std::get<1>(t->vs), (float)std::get<2>(t->vs) };
		f.spriteOffset = { 0, 0 };
		f.textureRect = { 0, 0, frame->spriteSize.x, frame->spriteSize.y };
		f.tex = std::move(t);

		//qid.texRectX = f.textureRect.x / f.textureRect.wh.x * 65535.f;
		//qid.texRectY = f.textureRect.y / f.textureRect.wh.y * 65535.f;
		//qid.texRectW = f.spriteSize.x / 65535.f
		//qid.texRectH = f.spriteSize.y / 65535.f
		// todo

		qid.texRectX = 	0;
		qid.texRectY = 	0;
		qid.texRectW = 	0xFFFFu;
		qid.texRectH = 	0xFFFFu;

		return *this;
	}

	Quad& Quad::SetFrame(Shared<Frame> f, bool overrideAnchor) {
		assert(!f->textureRotated);
		// todo
		//qid.texRectX = f->spriteOffset.x / 65535.f;
		//qid.texRectY = f->spriteOffset.y / 65535.f;
		//qid.texRectW = f->spriteSize.x / 65535.f;
		//qid.texRectH = f->spriteSize.y / 65535.f;
		frame = std::move(f);
		return *this;
	}

	XY& Quad::Size() const {
		assert(frame);
		return frame->spriteSize;
	}

	void Quad::Draw() const {
		engine.sm.GetShader<Shader_QuadInstance>().DrawQuad(*frame->tex, qid);
	}
}

void Logic10::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic10 Init( quad instance tests )" << std::endl;

	q.SetTexture( xx::engine.LoadTextureFromCache("res/mouse.pkm") );
}

int Logic10::Update() {
	q.Draw();

	return 0;
}
