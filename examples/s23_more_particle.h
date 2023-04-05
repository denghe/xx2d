#pragma once
#include "main.h"

namespace MoreParticleTest {

	struct Scene;
	struct ExplodeEffect {
		xx::Particle particle;
		xx::Quad ring;
		xx::XY pos;
		xx::Coros actions;
		xx::Coro Action_ScaleTo(float s, float step);
		xx::Coro Action_FadeOut(uint8_t step);
		xx::Coro Action_Explode();
		void Init(Scene* scene_, xx::XY const& pos_);
		bool Update();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		float timePool{};
		xx::Shared<xx::GLTexture> texRing, texStar;
		xx::Shared<xx::ParticleConfig> cfg;
		xx::ListLink<ExplodeEffect> ps;
		int32_t c{};
	};
}
