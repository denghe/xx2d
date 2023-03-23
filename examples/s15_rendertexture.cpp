#include "main.h"
#include "s15_rendertexture.h"

namespace RenderTextureTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "RenderTextureTest::Scene::Init" << std::endl;

		tex0 = xx::engine.LoadSharedTexture("res/tiledmap2/tree.png");

		xx::FrameBuffer fb(true);
		auto tex1 = fb.MakeTexture({ 1000, 500 }, true);
		fb.DrawTo(tex1, xx::RGBA8{ 20,20,20,255 }, [&] {});	// fill bg color only
		spr2.SetTexture(tex0);
		for (size_t i = 0; i < 100; i++) {
			fb.DrawTo(tex1, {}, [&] {
				spr2.SetPosition({ (float)rnd.Next(-450,450), (float)rnd.Next(-200,200) }).Draw();
			});
		}
		spr2.SetTexture(tex1).SetPosition({});
	}

	int Scene::Update() {

		xx::FrameBuffer fb(true);
		auto tex1 = fb.Draw({ 300, 400 }, true, xx::RGBA8{ 120,20,20,0 }, [&] {
			spr.SetTexture(tex0).SetScale(3).Draw();
		});
		auto tex2 = fb.Draw({ 300, 400 }, true, xx::RGBA8{}, [&] {
			spr.SetTexture(tex1).SetScale(1).Draw();
		});
		// performance test
		for (size_t i = 0; i < 10; i++) {
			tex1 = fb.Draw({ 300, 400 }, true, xx::RGBA8{}, [&] {
				spr.SetTexture(tex2).Draw();
			});
			tex2 = fb.Draw({ 300, 400 }, true, xx::RGBA8{}, [&] {
				spr.SetTexture(tex1).Draw();
			});
		}
		spr.SetTexture(tex2);

		spr2.Draw();
		spr.Draw();
		return 0;
	}

}
