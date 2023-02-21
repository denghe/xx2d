#include "pch.h"
#include "logic.h"
#include "logic13.h"

namespace MovePathTests {

	void Monster::Init(xx::XY const& pos, xx::Shared<xx::MovePathCache> mpc) {
		auto mp = mpc->Move(0);
		assert(mp);
		radians = mp->radians;
		this->originalPos = pos;
		this->pos = pos + mp->pos;
		this->mpc = std::move(mpc);
		DrawInit();
	}

	int Monster::Update() {
		movedDistance += speed;
		auto mp = mpc->Move(movedDistance);
		if (!mp) return 1;
		radians = mp->radians;
		pos = originalPos + mp->pos;
		return 0;
	}



	void Monster::DrawInit() {
		//body.FillCirclePoints({}, 16, 0.f, 12);
		body.SetTexture(owner->tex);
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

	void Scene::Init(Logic* logic) {
		this->logic = logic;
		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");

		coros.Add([](Scene* self)->xx::Coro {
			auto mp = xx::Make<xx::MovePath>();
			std::vector<xx::CurvePoint> cps;
			cps.emplace_back(xx::CurvePoint{ { 0, 0 }, 0.3f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 100, 0 }, 0.3f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 100, 100 }, 0.3f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 0, 100 }, 0.3f, 100 });
			mp->FillCurve(true, cps);

			auto mpc = xx::Make<xx::MovePathCache>();
			mpc->Init(mp, 1);

			auto hw = (int)xx::engine.hw;
			auto hh = (int)xx::engine.hh;
			auto r = 32;
			for (size_t j = 0; j < 10000; j++) {
				for (size_t i = 0; i < 10; i++) {
					auto&& m = self->AddMonster();
					xx::Pos v{ self->rnd.Next(-hw + r, hw - r), self->rnd.Next(-hh + r, hh - r) };
					m->Init(v.As<float>(), mpc);
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
}


void Logic13::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic13 Init( move path tests )" << std::endl;

	scene.Init(logic);

	auto secs = xx::NowSteadyEpochSeconds();
	size_t i = 0;
	for (; i < 100; i++) {
		scene.Update();
	}
	std::cout << "monsters.size == " << scene.monsters.size() << ". update " << i << " times. elapsed secs = " << xx::NowSteadyEpochSeconds(secs) << std::endl;
}


int Logic13::Update() {
	timePool += xx::engine.delta;
	auto timePoolBak = timePool;
	while (timePool >= 1.f / 120) {
		timePool -= 1.f / 120;

		scene.Update();
	}
	scene.Draw();
	return 0;
}
