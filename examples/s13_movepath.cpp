#include "main.h"
#include "s13_movepath.h"

namespace MovePath {

	void Monster::Init(xx::XY const& pos, xx::Shared<xx::MovePathCache> mpc, float const& speed, xx::RGBA8 const& color) {
		auto mp = mpc->Move(0);
		assert(mp);
		radians = mp->radians;
		this->originalPos = pos;
		this->pos = pos + mp->pos;
		this->mpc = std::move(mpc);
		this->speed = speed;
		DrawInit(color);
	}

	int Monster::Update() {
		movedDistance += speed;
		auto mp = mpc->Move(movedDistance);
		if (!mp) return 1;
		radians = mp->radians;
		pos = originalPos + mp->pos;
		return 0;
	}



	void Monster::DrawInit(xx::RGBA8 const& color) {
		//body.FillCirclePoints({}, 16, 0.f, 12);
		body.SetTexture(owner->tex).SetColor(color);
	}

	void Monster::Draw() {
		body.SetPosition(pos).SetRotate(-radians + float(M_PI / 2)).Draw();
	}



	xx::Shared<Monster>& Scene::AddMonster() {
		auto& m = monsters.emplace_back().Emplace();
		m->owner = this;
		m->indexAtOwnerMonsters = monsters.size() - 1;
		return m;
	}

	void Scene::EraseMonster(Monster* m) {
		assert(m);
		assert(m->owner);
		assert(m->owner == this);
		auto idx = m->indexAtOwnerMonsters;
		m->indexAtOwnerMonsters = std::numeric_limits<size_t>::max();
		m->owner = {};
		assert(monsters[idx] == m);
		monsters[idx] = monsters.back();
		monsters.back()->indexAtOwnerMonsters = idx;
		monsters.pop_back();
	}

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
		monsters.reserve(200000);

		coros.Add([](Scene* self)->xx::Coro {
			xx::MovePath mp;
			mp.FillCurve(true, { { 0, 0 }, { 200, 0 }, { 200, 100 }, { 0, 100 } });

			auto mpc = xx::Make<xx::MovePathCache>();
			mpc->Init(mp, 1);

			auto hw = (int)xx::engine.hw;
			auto hh = (int)xx::engine.hh;
			auto r = 32;
			for (size_t j = 0; j < 10000; j++) {
				for (size_t i = 0; i < 20; i++) {
					auto&& m = self->AddMonster();
					xx::Pos<> v{ self->rnd.Next(-hw + r, hw - r), self->rnd.Next(-hh + r, hh - r) };
					auto color = self->rnd.Get();
					m->Init(v.As<float>(), mpc, 2, (xx::RGBA8&)color);
				}
				CoYield;
			}
		}(this));
	}

	void Scene::Update() {
		coros();

		for (auto i = (ptrdiff_t)monsters.size() - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (m->Update()) {
				EraseMonster(m);
			}
		}
	}

	void Scene::Draw() {
		for (auto& m : monsters) {
			m->Draw();
		}
	}

	void Looper::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "MovePath::Looper::Init" << std::endl;

		scene.Init(looper);

		auto secs = xx::NowSteadyEpochSeconds();
		size_t i = 0;
		for (; i < 100; i++) {
			scene.Update();
		}
		std::cout << "monsters.size == " << scene.monsters.size() << ". update " << i << " times. elapsed secs = " << xx::NowSteadyEpochSeconds(secs) << std::endl;
	}

	int Looper::Update() {
		timePool += xx::engine.delta;
		auto timePoolBak = timePool;
		while (timePool >= 1.f / 120) {
			timePool -= 1.f / 120;

			scene.Update();
		}
		scene.Draw();
		return 0;
	}

}
