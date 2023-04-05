#pragma once
#include "main.h"

namespace MoreParticleTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		float timePool{};
		xx::Shared<xx::ParticleConfig> cfg;
		xx::ListLink<xx::Particle> ps;
		int32_t c{};
	};
}
