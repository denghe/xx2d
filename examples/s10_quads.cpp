#include "xx2d_pch.h"
#include "game_looper.h"
#include "s10_quads.h"

namespace Quads {

	void Mouse2::Init(Scene* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color) {
		body.SetTexture(owner->tex);
		body.pos = pos;
		body.scale = { scale, scale };
		body.radians = -radians + M_PI / 2;
		body.color = color;
		baseInc = { std::sin(body.radians), std::cos(body.radians) };
		baseInc *= 2;
	}

	int Mouse2::Update() {
		body.pos += baseInc;
		if (body.pos.x * body.pos.x > (1800 / 2) * (1800 / 2)
			|| body.pos.y * body.pos.y > (1000 / 2) * (1000 / 2)) return 1;
		return 0;
	}

	void Mouse2::Draw() {
		auto c = body.color;
		body.pos += xx::XY{ 3, 3 };
		body.color = { 255,127,127,127 };
		body.Draw();						// shadow
		body.pos -= xx::XY{ 3, 3 };
		body.color = c;
		body.Draw();
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "Quads::Scene::Init" << std::endl;

		tex = xx::engine.LoadTextureFromCache("res/sword.pkm.zstd");
	}

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			for (size_t i = 0; i < 100; i++) {
				radians += 0.005;
				ms.emplace_back().Emplace()->Init(this, {}, radians, 0.1);
			}

			for (auto i = (ptrdiff_t)ms.size() - 1; i >= 0; --i) {
				auto& m = ms[i];
				if (m->Update()) {
					m = ms.back();
					ms.pop_back();
				}
			}
		}

		for (auto& m : ms) {
			m->Draw();
		}

		return 0;
	}

}
