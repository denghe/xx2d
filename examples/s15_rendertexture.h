#pragma once
#include "main.h"

namespace RenderTextureTest {

	template<typename F>
	xx::Shared<xx::GLTexture> RenderToTexture(uint16_t const& w, uint16_t const& h, F&& func) {
		xx::engine.sm.End();
		auto bw = xx::engine.w;
		auto bh = xx::engine.h;
		xx::engine.w = w;
		xx::engine.h = h;
		auto [f, t] = xx::MakeGLFrameBuffer(w, h);
		glBindFramebuffer(GL_FRAMEBUFFER, f);
		glViewport(0, 0, w, h);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);	// todo: move to args
		glClear(GL_COLOR_BUFFER_BIT);

		func();

		xx::engine.sm.End();
		xx::engine.w = bw;
		xx::engine.h = bh;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, bw, bh);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		return xx::Make<xx::GLTexture>(std::move(t));
	}

	struct Scene : SceneBase {
		xx::Sprite spr, spr2;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
