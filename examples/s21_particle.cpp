#include "main.h"
#include "s21_particle.h"

namespace ParticleTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ParticalTest::Scene::Init" << std::endl;

        auto tex = xx::engine.LoadSharedTexture("res/particles/p11.png");
        xx::GLTexParm(*tex, GL_LINEAR);

        auto cfg = xx::Make<xx::ParticleConfig>();
        cfg->sprite.SetTexture(tex);
        cfg->emission = 10000;
        cfg->lifetime = -1.f;
        cfg->particleLife = { 2.f, 3.5f };
        cfg->direction = 0;
        cfg->spread = M_PI * 2;
        cfg->relative = {};
        cfg->speed = { -100.f, 300.f };
        cfg->gravity = { -200.f, 180.f };
        cfg->radialAccel = { -130.f, 200.f };
        cfg->tangentialAccel = { -200.f, 270.f };
        cfg->size = { 0.5f, 5.f };
        cfg->sizeVar = 0.4;
        cfg->spin = {};
        cfg->spinVar = 0;
        cfg->color = { { 1, 0, 0, 0.7 },{ 0, 1, 1, 0.3 } };
        cfg->colorVar = 0.5;
        cfg->alphaVar = 1;

        p.Init(cfg);
	}

	int Scene::Update() {

        p.FireAt(xx::engine.mousePosition);

        //p.rootPos = xx::engine.mousePosition;
        //p.Fire();

        p.Update(xx::engine.delta);
        p.Draw();
		return 0;
	}
}
