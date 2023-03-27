#include "xx2d.h"

namespace xx {

    void Particle::Init(xx::Shared<ParticleConfig> cfg_, size_t const& cap) {
        particles.resize(cap);
        cfg = std::move(cfg_);

        pos = prevPos = rootPos = {};
        scale = 1.0f;

        emissionResidue = {};
        particlesAlive = {};
        age = -2.0;
    }

    void Particle::Update(const float f_delta_time) {
        int i, e;
        float ang;

        if (age >= 0) {
            age += f_delta_time;
            if (age >= cfg->lifetime) {
                age = -2.0f;
            }
        }

        // update all alive particles

        auto par = particles.data();

        for (i = 0; i < particlesAlive; i++) {
            par->age += f_delta_time;
            if (par->age >= par->terminalAge) {
                particlesAlive--;
                memcpy(par, &particles[particlesAlive], sizeof(ParticleItem));
                i--;
                continue;
            }

            XY vecAccel = par->pos - pos;
            vecAccel.Normalize();
            XY vecAccel2 = vecAccel;
            vecAccel *= par->radialAccel;

            // vecAccel2.Rotate(M_PI_2);
            // the following is faster
            ang = vecAccel2.x;
            vecAccel2.x = -vecAccel2.y;
            vecAccel2.y = ang;

            vecAccel2 *= par->tangentialAccel;
            par->velocity += (vecAccel + vecAccel2) * f_delta_time;
            par->velocity.y += par->gravity * f_delta_time;

            par->pos += par->velocity * f_delta_time;

            par->spin += par->spinDelta * f_delta_time;
            par->size += par->sizeDelta * f_delta_time;
            par->color += par->colorDelta * f_delta_time;

            par++;
        }

        // generate new particles

        if (age != -2.0f) {
            const auto f_particles_needed = cfg->emission * f_delta_time + emissionResidue;
            const int nParticlesCreated = static_cast<unsigned int>(f_particles_needed);
            emissionResidue = f_particles_needed - nParticlesCreated;

            par = &particles[particlesAlive];

            for (i = 0, e = (int)particles.size(); i < nParticlesCreated; i++) {
                if (particlesAlive >= e) break;

                par->age = 0.0f;
                par->terminalAge = engine.rnd.Next(cfg->particleLife);

                par->pos = prevPos + (pos - prevPos) * engine.rnd.Next(0.f, 1.f) + XY{ engine.rnd.Next(-2.f, 2.f), engine.rnd.Next(-2.f, 2.f) };

                ang = cfg->direction - M_PI_2 + engine.rnd.Next(0.f, cfg->spread) - cfg->spread / 2.0f;
                if (cfg->relative) {
                    auto d = prevPos - pos;
                    ang += std::atan2(d.y, d.x) + M_PI_2;
                }
                par->velocity = XY{ std::cos(ang), std::sin(ang) } * engine.rnd.Next(cfg->speed);

                par->gravity = engine.rnd.Next(cfg->gravity);
                par->radialAccel = engine.rnd.Next(cfg->radialAccel);
                par->tangentialAccel = engine.rnd.Next(cfg->tangentialAccel);

                par->size = engine.rnd.Next(cfg->size.first, cfg->size.first + (cfg->size.second - cfg->size.first) * cfg->sizeVar);
                par->sizeDelta = (cfg->size.second - par->size) / par->terminalAge;

                par->spin = engine.rnd.Next(cfg->spin.first, cfg->spin.first + (cfg->spin.second - cfg->spin.first) * cfg->spinVar);
                par->spinDelta = (cfg->spin.second - par->spin) / par->terminalAge;

                par->color.r = engine.rnd.Next(cfg->color.first.r, cfg->color.first.r + (cfg->color.second.r - cfg-> color.first.r) * cfg->colorVar);
                par->color.g = engine.rnd.Next(cfg->color.first.g, cfg->color.first.g + (cfg->color.second.g - cfg-> color.first.g) * cfg->colorVar);
                par->color.b = engine.rnd.Next(cfg->color.first.b, cfg->color.first.b + (cfg->color.second.b - cfg-> color.first.b) * cfg->colorVar);
                par->color.a = engine.rnd.Next(cfg->color.first.a, cfg->color.first.a + (cfg->color.second.a - cfg-> color.first.a) * cfg->alphaVar);

                par->colorDelta = (cfg->color.second - par->color) / par->terminalAge;

                particlesAlive++;
                par++;
            }
        }

        prevPos = pos;
    }

    void Particle::MoveTo(XY const& xy, const bool b_move_particles) {

        if (b_move_particles) {
            const auto d = xy - pos;

            for (int i = 0; i < particlesAlive; i++) {
                particles[i].pos += d;
            }

            prevPos += d;
        } else {
            if (age == -2.0) {
                prevPos = xy;
            } else {
                prevPos = pos;
            }
        }

        pos = xy;
    }

    void Particle::FireAt(XY const& xy) {
        Stop();
        MoveTo(xy);
        Fire();
    }

    void Particle::Fire() {
        if (cfg->lifetime == -1.0f) {
            age = -1.0f;
        } else {
            age = 0.0f;
        }
    }

    void Particle::Stop(const bool b_kill_particles) {
        age = -2.0f;
        if (b_kill_particles) {
            particlesAlive = 0;
        }
    }

    void Particle::Draw() {
        std::pair<uint32_t, uint32_t> blendBak{};
        if (engine.blendFuncs.first != GL_SRC_ALPHA || engine.blendFuncs.second != GL_ONE) {
            engine.sm.End();
            blendBak = engine.blendFuncs;
            engine.GLBlendFunc({ GL_SRC_ALPHA, GL_ONE });
        }

        auto bak = cfg->sprite.color;

        auto par = particles.data();
        for (auto i = 0; i < particlesAlive; i++) {
            if (cfg->color.first.r < 0) {
                cfg->sprite.color.a = par->color.a * 255;
            } else {
                cfg->sprite.color = par->color;
            }
            cfg->sprite.SetPosition(par->pos * scale + rootPos)
                .SetScale(par->size * scale)
                .SetRotate(par->spin * par->age)
                .Draw();

            par++;
        }

        cfg->sprite.SetColor(bak);

        if (blendBak.first) {
            engine.sm.End();
            engine.GLBlendFunc(blendBak);
        }
    }
}
