#include "main.h"
#include "s23_more_particle.h"

namespace MoreParticleTest {

	xx::Coro ExplodeEffect::Action_ScaleTo(float s, float step) {
		CoYield;
		for (; ring.scale.x < s; ring.scale += step) {
			CoYield;
		}
		ring.scale = {s, s};
	}

	xx::Coro ExplodeEffect::Action_FadeOut(uint8_t step) {
		CoYield;
		for (; ring.color.a >= step; ring.color.a -= step) {
			CoYield;
		}
		ring.color.a = 0;
	}

	xx::Coro ExplodeEffect::Action_Explode() {
		particle.FireAt(pos);
		do {
			particle.Update(1.f / 60);
			CoYield;
		} while (!particle.Empty());
	}

	void ExplodeEffect::Init(Scene* scene_, xx::XY const& pos_) {
		pos = pos_;
		ring.SetPosition(pos).SetScale(0.01).SetTexture(scene_->cfg->sprite.tex);
		particle.Init(scene_->cfg, 2000);
		actions.Add(Action_ScaleTo(2.f, 2.f / 15));
		actions.Add(Action_FadeOut(255 / 15));
		actions.Add(Action_Explode());
	}

	bool ExplodeEffect::Update() {
		return !actions();
	}

	void ExplodeEffect::Draw() {
		ring.Draw();
		particle.Draw();
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ParticalTest::Scene::Init" << std::endl;

		auto tex = xx::engine.LoadSharedTexture("res/particles/p11.png");
		xx::GLTexParm(*tex, GL_LINEAR);

		cfg.Emplace();
		cfg->sprite.SetTexture(tex);
		cfg->emission = 5000;
		cfg->lifetime = 0.03f;
		cfg->particleLife = { 0.15f, 0.25f };
		cfg->direction = 0;
		cfg->spread = M_PI * 2;
		cfg->relative = {};
		cfg->speed = { -100.f, 150.f };
		cfg->gravity = { -200.f, 180.f };
		cfg->radialAccel = { -130.f, 200.f };
		cfg->tangentialAccel = { -200.f, 270.f };
		cfg->size = { 0.1f, 0.2f };
		cfg->sizeVar = 0.4;
		cfg->spin = {};
		cfg->spinVar = 0;
		cfg->color = { { 1, 0.3, 0.3, 1 },{ 1, 1, 1, 0.7 } };
		cfg->colorVar = 0.5;
		cfg->alphaVar = 1;
	}

	int Scene::Update() {

		// fixed frame rate logic
		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
		    timePool -= 1.f / 60;

		    if (xx::engine.Pressed(xx::Mbtns::Left)) {
		        auto& p = *new (&ps.Add()) ExplodeEffect();
				p.Init(this, xx::engine.mousePosition);
		    }

		    ps.Foreach([](auto& p) {
		        return p.Update();
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
