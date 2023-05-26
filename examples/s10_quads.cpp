#include "main.h"
#include "s10_quads.h"

namespace Quads {

	// https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors

	void Mouse::Init(Scene* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color) {
		body.SetTexture(owner->tex)
			.SetPosition(pos)
			.SetScale(scale)
			.SetRotate(-radians + M_PI / 2)
			.SetColor(color);
		baseInc = { std::sin(body.radians), std::cos(body.radians) };
		baseInc *= 2;
	}

	int Mouse::Update() {
		body.AddPosition(baseInc);
		if (body.pos.x * body.pos.x > (1800 / 2) * (1800 / 2)
			|| body.pos.y * body.pos.y > (1000 / 2) * (1000 / 2)) return 1;
		return 0;
	}

	void Mouse::Draw() {
		auto c = body.color;
		body.AddPosition({ 3,3 }).SetColor({ 255,127,127,127 }).Draw();	// shadow
		body.AddPosition({ -3,-3 }).SetColor(c).Draw();
	}

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "Quads::Scene::Init" << std::endl;

		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
	}

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			for (size_t i = 0; i < 50; i++) {
				radians += 0.005;
				ms.Emplace().Init(this, {}, radians);
			}

			ms.Foreach([](auto& m)->bool {
				return m.Update();
			});
		}

		ms.Foreach([](auto& m) {
			m.Draw();
		});

		return 0;
	}

}
