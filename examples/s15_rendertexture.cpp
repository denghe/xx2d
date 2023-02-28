#include "xx2d_pch.h"
#include "game_looper.h"
#include "s15_rendertexture.h"

namespace RenderTextureTest {

	void RenderTexture::Init(xx::Pos<> wh) {
		tex.Emplace();
		auto bytesPerRow = wh.x * 4;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8 - 4 * (wh.x & 0x1));
		auto data = malloc(bytesPerRow * wh.y);	// todo: free data?
		auto t = xx::GenBindGLTexture();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)wh.x, (GLsizei)wh.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
		CheckGLError();
		*tex = { t, wh.x, wh.y, "" };
		// todo
	}
	void RenderTexture::Begin() {
		xx::engine.sm.End();
		//GLint bak;
		//glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bak);
		// todo
	}
	void RenderTexture::End() {
		xx::engine.sm.End();
		// todo
		//glDeleteFramebuffers(1, &???);
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "Scene Init( render texture tests )" << std::endl;

		spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"))
			.SetScale(3);

		rt.Init({ 200, 200 });
		rt.Begin();
		spr.Draw();
		rt.End();

		//spr2.SetTexture(rt);	// todo
	}

	int Scene::Update() {
		spr2.Draw();
		return 0;
	}

}
