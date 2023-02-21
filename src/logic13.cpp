#include "pch.h"
#include "logic.h"
#include "logic13.h"

namespace MovePathTests {

	void MovePath::Fill(xx::XY const* ps, size_t len, bool loop) {
		assert(len > 1);
		totalDistance = {};
		this->loop = loop;
		if (ps) {
			points.resize(len);
			for (size_t i = 0; i < len; i++) {
				points[i].pos = ps[i];
			}
		} else {
			assert(len <= points.size());
		}
		for (size_t i = 0; i < len - 1; i++) {
			FillFields(points[i], points[i + 1]);
			totalDistance += points[i].distance;
		}
		if (loop) {
			FillFields(points[len - 1], points[0]);
			totalDistance += points[len - 1].distance;
		} else {
			points[len - 1].distance = {};
			points[len - 1].inc = {};
			points[len - 1].radians = points[len - 2].radians;
		}
	}

	void MovePath::Fill(bool loop) {
		Fill(nullptr, points.size(), loop);
	}

	void MovePath::FillFields(MovePathPoint& p1, MovePathPoint& p2) {
		auto v = p2.pos - p1.pos;
		p1.radians = std::atan2(v.y, v.x);
		p1.inc = { std::cos(p1.radians), std::sin(p1.radians) };
		p1.distance = std::sqrt(v.x * v.x + v.y * v.y);
	}

	void MovePathSteper::Init(xx::Shared<MovePath> mp) {
		this->mp = std::move(mp);
		cursor = {};
		cursorDistance = {};
	}

	MovePathSteper::MoveResult MovePathSteper::MoveToBegin() {
		assert(mp);
		assert(mp->points.size());
		cursor = {};
		cursorDistance = {};
		auto& p = mp->points.front();
		return { .pos = p.pos, .radians = p.radians, .movedDistance = {}, .terminated = false };
	}

	MovePathSteper::MoveResult MovePathSteper::MoveForward(float const& stepDistance) {
		assert(mp);
		assert(mp->points.size());
		auto& ps = mp->points;
		auto siz = ps.size();
		auto loop = mp->loop;
		auto d = stepDistance;
	LabLoop:
		auto& p = ps[cursor];
		auto left = p.distance - cursorDistance;
		if (d > left) {
			d -= left;
			cursorDistance = 0.f;
			++cursor;
			if (cursor == siz) {
				if (loop) {
					cursor = 0;
				} else {
					cursor = siz - 1;
					return { .pos = p.pos, .radians = p.radians, .movedDistance = stepDistance - d, .terminated = true };
				}
			}
			goto LabLoop;
		} else {
			cursorDistance += d;
		}
		return { .pos = p.pos + (p.inc * cursorDistance), .radians = p.radians, .movedDistance = stepDistance, .terminated = !mp->loop && cursor == siz - 1 };
	}

	void MovePathCache::Init(xx::Shared<MovePath> mp, float const& stepDistance) {
		assert(mp);
		assert(stepDistance > 0);
		assert(mp->totalDistance > stepDistance);
		this->stepDistance = stepDistance;
		this->loop = mp->loop;
		auto td = mp->totalDistance + stepDistance;
		points.clear();
		points.reserve(std::ceil(mp->totalDistance / stepDistance));
		MovePathSteper mpr;
		mpr.Init(std::move(mp));
		auto mr = mpr.MoveToBegin();
		points.push_back({ mr.pos, mr.radians });
		for (float d = stepDistance; d < td; d += stepDistance) {
			mr = mpr.MoveForward(stepDistance);
			points.push_back({ mr.pos, mr.radians });
		}
	}

	MovePathCachePoint* MovePathCache::Move(float const& totalDistance) {
		int i = totalDistance / stepDistance;
		if (loop) {
			return &points[i % points.size()];
		} else {
			return i < points.size() ? &points[i] : nullptr;
		}
	}

	void Monster::Init(xx::XY const& pos, xx::Shared<MovePathCache> mpc) {
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
			auto mp = xx::Make<MovePath>();
			mp->points.emplace_back().pos = { 0, 0 };
			mp->points.emplace_back().pos = { 100, 0 };
			mp->points.emplace_back().pos = { 100, 100 };
			mp->points.emplace_back().pos = { 0, 100 };
			mp->Fill(true);

			auto mpc = xx::Make<MovePathCache>();
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
