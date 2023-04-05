#include "main.h"
#include "s23_more_particle.h"

namespace MoreParticleTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ParticalTest::Scene::Init" << std::endl;

        auto tex = xx::engine.LoadSharedTexture("res/particles/p11.png");
        xx::GLTexParm(*tex, GL_LINEAR);

        cfg.Emplace();
        cfg->sprite.SetTexture(tex);
        cfg->emission = 5000;
        cfg->lifetime = 0.1f;
        cfg->particleLife = { 0.5f, 1.0f };
        cfg->direction = 0;
        cfg->spread = M_PI * 2;
        cfg->relative = {};
        cfg->speed = { -100.f, 300.f };
        cfg->gravity = { -200.f, 180.f };
        cfg->radialAccel = { -130.f, 200.f };
        cfg->tangentialAccel = { -200.f, 270.f };
        cfg->size = { 0.5f, 2.f };
        cfg->sizeVar = 0.4;
        cfg->spin = {};
        cfg->spinVar = 0;
        cfg->color = { { 1, 0, 0, 1 },{ 0, 1, 1, 0 } };
        cfg->colorVar = 0.5;
        cfg->alphaVar = 1;
	}

	int Scene::Update() {
        // fixed frame rate logic
        timePool += xx::engine.delta;
        while (timePool >= 1.f / 60) {
            timePool -= 1.f / 60;

            if (xx::engine.Pressed(xx::Mbtns::Left)) {
                auto& p = *new (&ps.Add()) xx::Particle();
                p.Init(cfg, 2000, { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
                p.FireAt(xx::engine.mousePosition);
            }

            ps.ForeachRemove([](auto& p) {
                p.Update(1.f / 60);
                return p.Empty();
            });
        }

        ps.Foreach([](auto& p) {
            p.Draw();
        });

        auto cc = ps.Count();
        if (c != cc) {
            c = cc;
            xx::CoutN(c);
        }

		return 0;
	}
}
