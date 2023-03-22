#pragma once
#include "main.h"

namespace RenderTextureTest {

	struct RenderTexture {
		xx::GLFrameBuffer fb;
		xx::Shared<xx::GLTexture> tex;
		void Init(xx::Pos<> wh);
		void Begin();
		void End();
	};

	template<typename F>
	xx::Shared<xx::GLTexture> RenderToTexture(F&& func) {
		auto [f, t] = xx::MakeGLFrameBuffer(xx::engine.w, xx::engine.h);
		xx::engine.sm.End();

		glBindFramebuffer(GL_FRAMEBUFFER, f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		xx::engine.sm.GLInit();

		func();

		xx::engine.sm.End();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		xx::engine.sm.GLInit();

		return xx::Make<xx::GLTexture>(std::move(t));
	}

	struct Scene : SceneBase {
		xx::Sprite spr, spr2;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
