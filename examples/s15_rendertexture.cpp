#include "main.h"
#include "s15_rendertexture.h"

namespace RenderTextureTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "RenderTextureTest::Scene::Init" << std::endl;

		spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"))
			.SetScale(3);

		auto tex = RenderToTexture(300, 500, [this] {
			spr.Draw();
		});
		spr2.SetTexture(tex).SetFlipY(true);

	}

	int Scene::Update() {
		spr2.Draw();
		return 0;
	}

}
