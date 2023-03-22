#include "main.h"
#include "s15_rendertexture.h"

namespace RenderTextureTest {

	void RenderTexture::Init(xx::Pos<> wh) {
		auto&& r = xx::MakeGLFrameBuffer(wh.x, wh.y);
		fb = std::move(r.first);
		tex.Emplace(std::move(r.second));
	}

	void RenderTexture::Begin() {
		xx::engine.sm.End();

		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		xx::engine.sm.GLInit();
	}

	void RenderTexture::End() {
		xx::engine.sm.End();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		xx::engine.sm.GLInit();
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "RenderTextureTest::Scene::Init" << std::endl;


		//RenderTexture rt;
		//rt.Init({ (int)xx::engine.w, (int)xx::engine.h });
		//rt.Begin();
		//spr.Draw();
		//rt.End();
		//auto tex = std::move(rt.tex);
		//spr2.SetTexture(tex);


		// todo: set current frame buffer width, height to shader ? 
		// todo: flip y ??

		spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"))
			.SetScale(3);

		auto tex = RenderToTexture([this] {
			spr.Draw();
		});
		spr2.SetTexture(tex).SetFlipY(true);

	}

	int Scene::Update() {
		spr2.Draw();
		return 0;
	}

}
