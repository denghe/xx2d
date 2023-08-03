#include "main.h"
#include "s14_scissor.h"

namespace Scissor {

	void GLScissor::Begin() {
		xx::engine.sm.End();
		glEnable(GL_SCISSOR_TEST);
		glScissor(xx::engine.hw + rect.x, xx::engine.hh + rect.y, rect.wh.x, rect.wh.y);
	}
	void GLScissor::End() {
		xx::engine.sm.End();
		glDisable(GL_SCISSOR_TEST);
	}

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "Scissor::Scene::Init" << std::endl;

		spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"))
			.SetScale(3);

		scissor.rect = { -100, -100, 200, 200 };

		tasks.AddTask([](Scene* self)->xx::Task<> {
			float ay = 0;
			while (true) {
				for (; ay < 1.f; ay += 0.001f) {
					self->spr.SetAnchor({ 0.5, ay });
					co_yield 0;
				}
				for (; ay >= 0; ay -= 0.001f) {
					self->spr.SetAnchor({ 0.5, ay });
					co_yield 0;
				}
			}
			}(this));
	}

	int Scene::Update() {
		tasks();
		scissor.Begin();
		spr.Draw();
		scissor.End();
		return 0;
	}

}
