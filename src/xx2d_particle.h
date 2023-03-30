﻿#pragma once
#include "xx2d.h"
#include "xx_listlink.h"

namespace xx {

	// code ref from HGE

	struct ParticleItem {
		XY pos, velocity;
		float gravity, radialAccel, tangentialAccel, spin, spinDelta, size, sizeDelta, age, terminalAge;
		RGBA color, colorDelta; // + alpha
	};

	struct ParticleConfig {
		Quad sprite;

		int emission;	// 0 ~ 1000
		float lifetime;	// 0 ~ 10s, -1.0f

		std::pair<float, float> particleLife;	// 0 ~ 5

		float direction;	// 0 ~ 2PI
		float spread;	// 0 ~ 2PI
		int relative;	// 0, 1

		std::pair<float, float> speed;	// -300 ~ 300
		std::pair<float, float> gravity;	// -900 ~ 900
		std::pair<float, float> radialAccel;	// -900 ~ 900
		std::pair<float, float> tangentialAccel;	// -900 ~ 900
		std::pair<float, float> size;	// 1 ~ 100
		float sizeVar;	// 0 ~ 1

		std::pair<float, float> spin;	// -50, 50
		float spinVar;	// 0 ~ 1

		std::pair<RGBA, RGBA> color; // + alpha
		float colorVar, alphaVar;	// 0 ~ 1
	};

	struct Particle {

		void Init(xx::Shared<ParticleConfig> cfg_, size_t const& cap = 1000, std::pair<uint32_t, uint32_t> blendFuncs_ = { GL_ONE, GL_ONE_MINUS_SRC_ALPHA });
		void Update(float delta);

		void FireAt(XY const& xy);
		void Fire();
		void MoveTo(XY const& xy, bool moveParticles = false);
		void Stop(bool killParticles = false);

		void Draw();

		xx::Shared<ParticleConfig> cfg;
		std::pair<uint32_t, uint32_t> blendFuncs;

		xx::ListLink<ParticleItem, int> particles;

		float age, emissionResidue, scale;
		XY prevPos, pos, rootPos;
	};
}
