﻿#include "xx2d.h"

namespace xx {
	void FrameBuffer::Init() {
		if (fb) {
			throw std::logic_error("excessive initializations ?");
		}
		fb = MakeGLFrameBuffer();
	}

	xx::Shared<GLTexture> FrameBuffer::MakeTex(xx::Pos<> const& wh, bool const& hasAlpha) {
		return xx::Make<GLTexture>(MakeGLFrameBufferTexture(wh.x, wh.y, hasAlpha));
	}

	void FrameBuffer::Begin(xx::Shared<GLTexture>& t, RGBA8 const& c) {
		xx::engine.sm.End();
		bak.x = xx::engine.w;
		bak.y = xx::engine.h;
		xx::engine.w = std::get<1>(t->vs);
		xx::engine.h = std::get<2>(t->vs);
		BindGLFrameBufferTexture(fb, *t);
		engine.GLClear(c);
	}

	void FrameBuffer::End() {
		xx::engine.sm.End();
		UnbindGLFrameBuffer();
		xx::engine.w = bak.x;
		xx::engine.h = bak.y;
		engine.GLClear();
	}
}
