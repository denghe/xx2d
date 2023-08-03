#pragma once
#include "main.h"

namespace MoreParticleTest {

	struct Scene;

	// important: memory can't move !!!  container need reserve !!!
	struct ExplodeEffect {
		xx::Particle particle;
		xx::Quad ring;
		xx::XY pos;
		xx::Tasks actions;
		xx::Task<> Action_ScaleTo(float s, float step);
		xx::Task<> Action_FadeOut(uint8_t step);
		xx::Task<> Action_Explode();
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
		xx::ListLink<ExplodeEffect, int, 0> ps;
		int32_t c{};
	};
}
