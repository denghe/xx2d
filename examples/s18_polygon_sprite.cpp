#include "xx2d_pch.h"
#include "game_looper.h"
#include "s18_polygon_sprite.h"

namespace PolygonSpriteTest {

	void Tree::Init(Scene* owner, xx::XY const& pos, float const& radians, float const& scale, xx::RGBA8 const& color) {
		body.SetFrame(owner->f)
			.SetPosition(pos)
			.SetScale(scale)
			.SetRotate(-radians + M_PI / 2)
			.SetColor(color);
		baseInc = { std::sin(body.radians), std::cos(body.radians) };
		baseInc *= 2;
	}

	int Tree::Update() {
		body.AddPosition(baseInc);
		if (body.pos.x * body.pos.x > (1800 / 2) * (1800 / 2)
			|| body.pos.y * body.pos.y > (1000 / 2) * (1000 / 2)) return 1;
		return 0;
	}

	void Tree::Draw() {
		auto c = body.color;
		body.AddPosition({ 3,3 }).SetColor({ 255,127,127,127 }).Draw();
		body.AddPosition({ -3,-3 }).SetColor(c).Draw();
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "PolygonSpriteTest::Scene::Init" << std::endl;

		xx::TP tp;
		tp.Fill("res/sword.plist");
		assert(tp.frames.size());
		f = tp.frames[0];
	}

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			for (size_t i = 0; i < 100; i++) {
				radians += 0.005;
				ts.emplace_back().Emplace()->Init(this, {}, radians, 0.1);
			}

			for (auto i = (ptrdiff_t)ts.size() - 1; i >= 0; --i) {
				auto& o = ts[i];
				if (o->Update()) {
					o = ts.back();
					ts.pop_back();
				}
			}
		}

		for (auto& o : ts) {
			o->Draw();
		}

		return 0;
	}

}
