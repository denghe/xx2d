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
		}
		else {
			assert(len <= points.size());
		}
		for (size_t i = 0; i < len - 1; i++) {
			FillFields(points[i], points[i + 1]);
			totalDistance += points[i].distance;
		}
		if (loop) {
			FillFields(points[len - 1], points[0]);
		}
		else {
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
		p1.radians = std::atan2(v.x, v.y);
		p1.inc = { std::sin(p1.radians), std::sin(p1.radians) };
		p1.distance = std::sqrt(v.x * v.x + v.y * v.y);
	}

	void MovePathRunner::Init(xx::Shared<MovePath> mp) {
		this->mp = std::move(mp);
		cursor = {};
		cursorDistance = {};
	}

	MovePathRunner::MoveResult MovePathRunner::MoveToBegin() {
		assert(mp);
		assert(mp->points.size());
		cursor = {};
		cursorDistance = {};
		auto& p = mp->points.front();
		return { .pos = p.pos, .radians = p.radians, .movedDistance = {}, .terminated = false };
	}

	MovePathRunner::MoveResult MovePathRunner::MoveToEnd() {
		assert(mp);
		assert(mp->points.size());
		cursor = {};
		cursorDistance = {};
		auto& p = mp->points.back();
		return { .pos = p.pos, .radians = p.radians, .movedDistance = mp->totalDistance, .terminated = !mp->loop };
	}

	MovePathRunner::MoveResult MovePathRunner::MoveForward(float distance) {
		assert(mp);
		// todo
		return MoveToEnd();
	}

	MovePathRunner::MoveResult MovePathRunner::MoveBackward(float distance) {
		assert(mp);
		// todo
		return MoveToBegin();
	}



	void Monster::Init(xx::XY const& pos, xx::Shared<MovePath> mp) {
		this->pos = pos;
		radians = mp->points.front().radians;
		fixedPos = pos;
		mpr.Init(std::move(mp));
		DrawInit();
	}

	int Monster::Update() {
		auto mr = mpr.MoveForward(speed * xx::engine.delta);
		if (mr.terminated) return 1;
		// mr.movedDistance for calc anim
		radians = mr.radians;
		fixedPos = pos + mr.pos;
		return 0;
	}



	void Monster::DrawInit() {
		body.FillCirclePoints({}, 32, 0.f, 24);
	}

	void Monster::Draw() {
		body.SetPosition(fixedPos).SetRotate(radians).Draw();
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

		// todo: scene logic coro?

		auto mp = xx::Make<MovePath>();
		mp->points.emplace_back().pos = { 0, 0 };
		mp->points.emplace_back().pos = { 100, 0 };
		mp->points.emplace_back().pos = { 200, 100 };
		mp->points.emplace_back().pos = { 200, 200 };
		mp->points.emplace_back().pos = { 100, 100 };
		mp->points.emplace_back().pos = { 0, 100 };
		mp->Fill(true);

		auto&& m = AddMonster();
		m->Init({ -100, -50 }, mp);
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
