#pragma once
#include "main.h"

namespace xx {

	// code copy from HGE

	struct RGBA {
		float r, g, b, a;
		RGBA operator*(IsArithmetic auto const& v) const {
			return { r * v, g * v, b * v, a * v };
		}
		RGBA& operator+=(RGBA const& v) {
			r += v.r;
			g += v.g;
			b += v.b;
			a += v.a;
			return *this;
		}
		operator RGBA8() const {
			return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
		}
	};

	struct ParticleItem {
		XY vecLocation;
		XY vecVelocity;

		float fGravity;
		float fRadialAccel;
		float fTangentialAccel;

		float fSpin;
		float fSpinDelta;

		float fSize;
		float fSizeDelta;

		RGBA colColor; // + alpha
		RGBA colColorDelta;

		float fAge;
		float fTerminalAge;
	};

	struct ParticleConfig {
		Quad sprite;

		int nEmission;	// 0 ~ 1000
		float fLifetime;	// 0 ~ 10s, -1.0f

		float fParticleLifeMin;	// 0 ~ 5
		float fParticleLifeMax;	// 0 ~ 5

		float fDirection;	// 0 ~ 2PI
		float fSpread;	// 0 ~ 2PI
		bool bRelative;

		float fSpeedMin;	// -300 ~ 300
		float fSpeedMax;	// -300 ~ 300

		float fGravityMin;	// -900 ~ 900
		float fGravityMax;	// -900 ~ 900

		float fRadialAccelMin;	// -900 ~ 900
		float fRadialAccelMax;	// -900 ~ 900

		float fTangentialAccelMin;	// -900 ~ 900
		float fTangentialAccelMax;	// -900 ~ 900

		float fSizeStart;	// 1 ~ 100
		float fSizeEnd;	// 1 ~ 100
		float fSizeVar;	// 0 ~ 1

		float fSpinStart;	// -50, 50
		float fSpinEnd;	// -50, 50
		float fSpinVar;	// 0 ~ 1

		RGBA colColorStart; // + alpha
		RGBA colColorEnd;
		float fColorVar;	// 0 ~ 1
		float fAlphaVar;	// 0 ~ 1
	};

	struct Particle {

		void Init(xx::Shared<ParticleConfig> cfg_);
		void Draw();
		void FireAt(XY const& xy);
		void Fire();
		void Stop(bool bKillParticles = false);
		void Update(float fDeltaTime);
		void MoveTo(XY const& xy, bool bMoveParticles = false);


		xx::Shared<ParticleConfig> cfg;

		float age_;
		float emission_residue_;

		XY prev_location_;
		XY location_;
		XY rootPos;
		float scale_;

		int particles_alive_;

		inline static const constexpr size_t MAX_PARTICLES = 100000;
		ParticleItem particles_[MAX_PARTICLES];
	};

}

namespace ParticalTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;


		xx::Particle p;
	};
}
