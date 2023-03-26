#include "main.h"
#include "s21_partical.h"

namespace xx {

    void Particle::Init(xx::Shared<ParticleConfig> cfg_) {
        cfg = std::move(cfg_);

        location_.x = prev_location_.x = 0.0f;
        location_.y = prev_location_.y = 0.0f;
        rootPos = {};
        scale_ = 1.0f;

        emission_residue_ = 0.0f;
        particles_alive_ = 0;
        age_ = -2.0;
    }

    void Particle::Update(const float f_delta_time) {
        int i;
        float ang;

        if (age_ >= 0) {
            age_ += f_delta_time;
            if (age_ >= cfg->fLifetime) {
                age_ = -2.0f;
            }
        }

        // update all alive particles

        ParticleItem* par = particles_;

        for (i = 0; i < particles_alive_; i++) {
            par->fAge += f_delta_time;
            if (par->fAge >= par->fTerminalAge) {
                particles_alive_--;
                memcpy(par, &particles_[particles_alive_], sizeof(ParticleItem));
                i--;
                continue;
            }

            XY vecAccel = par->vecLocation - location_;
            vecAccel.Normalize();
            XY vecAccel2 = vecAccel;
            vecAccel *= par->fRadialAccel;

            // vecAccel2.Rotate(M_PI_2);
            // the following is faster
            ang = vecAccel2.x;
            vecAccel2.x = -vecAccel2.y;
            vecAccel2.y = ang;

            vecAccel2 *= par->fTangentialAccel;
            par->vecVelocity += (vecAccel + vecAccel2) * f_delta_time;
            par->vecVelocity.y += par->fGravity * f_delta_time;

            par->vecLocation += par->vecVelocity * f_delta_time;

            par->fSpin += par->fSpinDelta * f_delta_time;
            par->fSize += par->fSizeDelta * f_delta_time;
            par->colColor += par->colColorDelta * f_delta_time;

            par++;
        }

        // generate new particles

        if (age_ != -2.0f) {
            const auto f_particles_needed = cfg->nEmission * f_delta_time + emission_residue_;
            const int nParticlesCreated = static_cast<unsigned int>(f_particles_needed);
            emission_residue_ = f_particles_needed - nParticlesCreated;

            par = &particles_[particles_alive_];

            for (i = 0; i < nParticlesCreated; i++) {
                if (particles_alive_ >= MAX_PARTICLES) {
                    break;
                }

                par->fAge = 0.0f;
                par->fTerminalAge = engine.rnd.Next<float>(cfg->fParticleLifeMin, cfg->fParticleLifeMax);

                par->vecLocation = prev_location_ + (location_ - prev_location_) * engine.rnd.Next<float>(0.0f, 1.0f);
                par->vecLocation.x += engine.rnd.Next<float>(-2.0f, 2.0f);
                par->vecLocation.y += engine.rnd.Next<float>(-2.0f, 2.0f);

                ang = cfg->fDirection - M_PI_2 + engine.rnd.Next<float>(0, cfg->fSpread) - cfg->fSpread /
                    2.0f;
                if (cfg->bRelative) {
                    auto d = prev_location_ - location_;
                    ang += std::atan2(d.y, d.x) + M_PI_2;
                }
                par->vecVelocity.x = cosf(ang);
                par->vecVelocity.y = sinf(ang);
                par->vecVelocity *= engine.rnd.Next<float>(cfg->fSpeedMin, cfg->fSpeedMax);

                par->fGravity = engine.rnd.Next<float>(cfg->fGravityMin, cfg->fGravityMax);
                par->fRadialAccel = engine.rnd.Next<float>(cfg->fRadialAccelMin, cfg->fRadialAccelMax);
                par->fTangentialAccel = engine.rnd.Next<float>(cfg->fTangentialAccelMin,
                    cfg->fTangentialAccelMax);

                par->fSize = engine.rnd.Next<float>(cfg->fSizeStart,
                    cfg->fSizeStart + (cfg->fSizeEnd - cfg->fSizeStart) *
                    cfg->fSizeVar);
                par->fSizeDelta = (cfg->fSizeEnd - par->fSize) / par->fTerminalAge;

                par->fSpin = engine.rnd.Next<float>(cfg->fSpinStart,
                    cfg->fSpinStart + (cfg->fSpinEnd - cfg->fSpinStart) *
                    cfg->fSpinVar);
                par->fSpinDelta = (cfg->fSpinEnd - par->fSpin) / par->fTerminalAge;

                par->colColor.r = engine.rnd.Next<float>(cfg->colColorStart.r,
                    cfg->colColorStart.r + (cfg->colColorEnd.r - cfg->
                        colColorStart.r) * cfg->fColorVar);
                par->colColor.g = engine.rnd.Next<float>(cfg->colColorStart.g,
                    cfg->colColorStart.g + (cfg->colColorEnd.g - cfg->
                        colColorStart.g) * cfg->fColorVar);
                par->colColor.b = engine.rnd.Next<float>(cfg->colColorStart.b,
                    cfg->colColorStart.b + (cfg->colColorEnd.b - cfg->
                        colColorStart.b) * cfg->fColorVar);
                par->colColor.a = engine.rnd.Next<float>(cfg->colColorStart.a,
                    cfg->colColorStart.a + (cfg->colColorEnd.a - cfg->
                        colColorStart.a) * cfg->fAlphaVar);

                par->colColorDelta.r = (cfg->colColorEnd.r - par->colColor.r) / par->fTerminalAge;
                par->colColorDelta.g = (cfg->colColorEnd.g - par->colColor.g) / par->fTerminalAge;
                par->colColorDelta.b = (cfg->colColorEnd.b - par->colColor.b) / par->fTerminalAge;
                par->colColorDelta.a = (cfg->colColorEnd.a - par->colColor.a) / par->fTerminalAge;

                particles_alive_++;
                par++;
            }
        }

        prev_location_ = location_;
    }

    void Particle::MoveTo(XY const& xy, const bool b_move_particles) {

        if (b_move_particles) {
            const auto d = xy - location_;

            for (int i = 0; i < particles_alive_; i++) {
                particles_[i].vecLocation += d;
            }

            prev_location_ += d;
        } else {
            if (age_ == -2.0) {
                prev_location_ = xy;
            } else {
                prev_location_ = location_;
            }
        }

        location_ = xy;
    }

    void Particle::FireAt(XY const& xy) {
        Stop();
        MoveTo(xy);
        Fire();
    }

    void Particle::Fire() {
        if (cfg->fLifetime == -1.0f) {
            age_ = -1.0f;
        } else {
            age_ = 0.0f;
        }
    }

    void Particle::Stop(const bool b_kill_particles) {
        age_ = -2.0f;
        if (b_kill_particles) {
            particles_alive_ = 0;
        }
    }

    void Particle::Draw() {
        bool needRestoreBlendFunc = engine.GLBlendFunc(GL_SRC_ALPHA, GL_ONE);

        auto par = particles_;

        const auto bak = cfg->sprite.color;

        for (auto i = 0; i < particles_alive_; i++) {
            if (cfg->colColorStart.r < 0) {
                cfg->sprite.color.a = par->colColor.a * 255;
            } else {
                cfg->sprite.color = par->colColor;
            }
            cfg->sprite.SetPosition(par->vecLocation * scale_ + rootPos)
                .SetScale(par->fSize * scale_)
                .SetRotate(par->fSpin * par->fAge)
                .Draw();

            par++;
        }

        cfg->sprite.SetColor(bak);

        if (needRestoreBlendFunc) {
            engine.GLBlendFunc();
        }
    }
}

namespace ParticalTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ParticalTest::Scene::Init" << std::endl;

        auto tex = xx::engine.LoadSharedTexture("res/particles/p11.png");
        xx::GLTexParm(*tex, GL_LINEAR);

        auto cfg = xx::Make<xx::ParticleConfig>();
        cfg->sprite.SetTexture(tex);
        cfg->nEmission = 10000;
        cfg->fLifetime = -1.f;
        cfg->fParticleLifeMin = 3.5;
        cfg->fParticleLifeMax = 2;
        cfg->fDirection = 0;
        cfg->fSpread = M_PI * 2;
        cfg->bRelative = false;
        cfg->fSpeedMin = -100;
        cfg->fSpeedMax = 300;
        cfg->fGravityMin = -200;
        cfg->fGravityMax = 180;
        cfg->fRadialAccelMin = -130;
        cfg->fRadialAccelMax = 200;
        cfg->fTangentialAccelMin = -200;
        cfg->fTangentialAccelMax = 270;
        cfg->fSizeStart = 5;
        cfg->fSizeEnd = 0.5;
        cfg->fSizeVar = 0.4;
        cfg->fSpinStart = 0;
        cfg->fSpinEnd = 0;
        cfg->fSpinVar = 0;
        cfg->colColorStart = { 1, 0, 0, 0.7 };
        cfg->colColorEnd = { 0, 1, 1, 0.3 };
        cfg->fColorVar = 0.5;
        cfg->fAlphaVar = 1;

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
