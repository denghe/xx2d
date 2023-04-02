#pragma once
#include "main.h"

namespace ParticleTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;


		xx::Particle p;
	};
}
