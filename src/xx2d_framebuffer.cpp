#include "xx2d.h"

namespace xx {
	FrameBuffer& FrameBuffer::Init() {
		if (fb) {
			throw std::logic_error("excessive initializations ?");
		}
		fb = MakeGLFrameBuffer();
		return *this;
	}

	xx::Shared<GLTexture> FrameBuffer::MakeTexture(xx::Pos<uint32_t> const& wh, bool const& hasAlpha) {
		return xx::MakeShared<GLTexture>(MakeGLFrameBufferTexture(wh.x, wh.y, hasAlpha));
	}

	void FrameBuffer::Begin(xx::Shared<GLTexture>& t, std::optional<RGBA8> const& c) {
		xx::engine.sm.End();
		bak.x = xx::engine.w;
		bak.y = xx::engine.h;
		xx::engine.w = t->Width();
		xx::engine.h = t->Height();
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
