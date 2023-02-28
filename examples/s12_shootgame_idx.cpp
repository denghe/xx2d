#include "xx2d_pch.h"
#include "game_looper.h"
#include "s12_shootgame_idx.h"

//#define ENABLE_DEBUG_DRAW

namespace ShootGameWithIndex {

	/*************************************************************************/
	// scene
	/*************************************************************************/

	void Scene::EraseMonster(int idx) {
		assert(idx != -1);
		monsters[idx]->indexAtMonsters = -1;
		monsters.back()->indexAtMonsters = idx;
		monsters[idx] = std::move(monsters.back());
		monsters.pop_back();
	}
	void Scene::EraseMonster(Monster* const& m) {
		assert(m);
		EraseMonster(m->indexAtMonsters);
	}

	void Scene::Init() {
		fnt.Load("res/font2/basechars.fnt");
		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");

		sgMonsters.Init(400, 400, 32);

		player.Emplace()->Init(this);

		running = true;
		coros.Add([](Scene* scene)->xx::Coro {
			while (scene->running) {
				for (size_t i = 0; i < 20; ++i) {
					scene->monsters.emplace_back().Emplace()->Init(scene);
				}
				CoYield;
			}
		}(this));
	}

	void Scene::Update() {

		// call scene looper
		coros();

		/*********************************************/
		// move monsters

		for (auto i = (ptrdiff_t)monsters.size() - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (!m->Update()) {
				EraseMonster(m);
			}
		}

		/*********************************************/
		// move bullets & players

		player->Update();

		/*********************************************/
		// move labels

		for (auto i = (ptrdiff_t)labels.size() - 1; i >= 0; --i) {
			auto& o = labels[i];
			if (!o->Update()) {
				o = labels.back();
				labels.pop_back();
			}
		}

	}

	void Scene::Draw() {
		// looper data -> display component
		player->DrawCommit();
		tmp.push_back(&player->body);

		for (auto& b : player->bullets) {
			b->DrawCommit();
			tmp.push_back(&b->body);
		}
		for (auto& m : monsters) {
			m->DrawCommit();
			tmp.push_back(&m->body);
		}

		// draw tex: sort by y
		std::sort(tmp.begin(), tmp.end(), [](auto const& a, auto const& b) {
			return a->pos.y > b->pos.y;
			});
		for (auto& q : tmp) {
			q->Draw();
		}
		tmp.clear();

		// draw label
		for (auto& l : labels) {
			l->DrawCommit();
			l->lbl.Draw();
		}

#ifdef ENABLE_DEBUG_DRAW
		// draw debug rings
		player->ring.Draw();
		for (auto& b : player->bullets) {
			b->ring.Draw();
		}
		for (auto& m : monsters) {
			m->ring.Draw();
		}
#endif
	}

	/*************************************************************************/
	// player
	/*************************************************************************/

	void Player::Init(Scene* scene) {
		this->scene = scene;
		DrawInit();
	}

	void Player::Update() {

		/*********************************************/
		// move bullets

		for (auto i = (ptrdiff_t)bullets.size() - 1; i >= 0; --i) {
			auto& b = bullets[i];
			if (!b->Update()) {
				b = bullets.back();
				bullets.pop_back();
			}
		}

		/*********************************************/
		// player move control

		union Dirty {
			struct {
				uint8_t a, s, d, w;
			};
			uint32_t all{};
		} flags;
		int n = 0;

		if (xx::engine.Pressed(xx::KbdKeys::A)) { flags.a = 1; ++n; }
		if (xx::engine.Pressed(xx::KbdKeys::S)) { flags.s = 1; ++n; }
		if (xx::engine.Pressed(xx::KbdKeys::D)) { flags.d = 1; ++n; }
		if (xx::engine.Pressed(xx::KbdKeys::W)) { flags.w = 1; ++n; }

		if (n > 2) {
			if (flags.a && flags.d) {
				flags.a = flags.d == 0;
				n -= 2;
			}
			if (flags.s && flags.w) {
				flags.s = flags.s == 0;
				n -= 2;
			}
		}

		xx::XY v{};	// vec2

		if (n == 2) {
			if (flags.w) {
				if (flags.d) {
					v = { M_SQ, -M_SQ };	// up right
				} else if (flags.a) {
					v = { -M_SQ, -M_SQ };	// up left
				}
			} else if (flags.s) {
				if (flags.d) {
					v = { M_SQ, M_SQ };	// right down
				} else if (flags.a) {
					v = { -M_SQ, M_SQ };	// left down
				}
			}
		} else if (n == 1) {
			if (flags.w) {
				v.y = -1;	// up
			} else if (flags.s) {
				v.y = 1;	// down
			} else if (flags.a) {
				v.x = -1;	// left
			} else if (flags.d) {
				v.x = 1;	// right
			}
		}

		pos += v * speed;

		/*********************************************/
		// player shoot control

		if (xx::engine.Pressed(xx::Mbtns::Left)) {
			auto mpos = xx::engine.mousePosition.MakeFlipY();
			v = (mpos - pos);// .Normalize();
			auto r = std::atan2(v.x, v.y);
			bullets.emplace_back().Emplace()->Init(scene, this, { std::sin(r), std::cos(r) }, 1000);
			for (size_t i = 1; i <= 10; ++i) {
				auto r1 = r + 0.05f * (float)i;
				bullets.emplace_back().Emplace()->Init(scene, this, { std::sin(r1), std::cos(r1) }, 1000);
				auto r2 = r - 0.05f * (float)i;
				bullets.emplace_back().Emplace()->Init(scene, this, { std::sin(r2), std::cos(r2) }, 1000);
			}
		}
	}

	void Player::DrawInit() {
		body.SetTexture(scene->tex).SetAnchor({ 0.5, 0.25 }).SetScale(radius * 3 / body.Size().x);
#ifdef ENABLE_DEBUG_DRAW
		ring.FillCirclePoints({}, radius, {}, 16).SetColor({ 255,0,0,127 });
#endif
	}
	void Player::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
#ifdef ENABLE_DEBUG_DRAW
		ring.SetPosition(p);
#endif
	}

	/*************************************************************************/
	// bullet
	/*************************************************************************/

	void Bullet::Init(Scene* const& scene, Player* const& owner, xx::XY const& inc, int const& life) {
		this->scene = scene;
		this->owner = owner;

		this->life = life;
		this->radius = owner->bulletRadius;
		this->speed = owner->bulletSpeed;
		this->inc = inc;
		this->pos = owner->pos + inc * (owner->radius + radius);
		DrawInit();
	}

	int Bullet::Update() {

		auto& sg = scene->sgMonsters;
		auto p = pos.As<int32_t>() + 6400;
		auto rcIdx = p / sg.maxDiameter;
		auto idx = rcIdx.y * sg.numCols + rcIdx.x;
		Monster* r{};
		int limit = 0x7FFFFFFF;
		scene->sgMonsters.Foreach9NeighborCells<true>(idx, [&](Monster* const& m) {
			auto d = m->pos - pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (dd < rr) {
				r = m;
				limit = 0;	// break foreach
			}
		}, &limit);
		if (r) {
			scene->labels.emplace_back().Emplace()->Init(scene, r->pos, r->radius);	// pop up damage hp
			scene->EraseMonster(r);
			return 0;
		}

		pos += inc * speed;
		return --life;
	}

	void Bullet::DrawInit() {
		body.SetTexture(scene->tex).SetColor({ 255,0,0,127 }).SetRotate(std::atan2(inc.y, inc.x) + M_PI / 2);
#ifdef ENABLE_DEBUG_DRAW
		ring.FillCirclePoints({}, radius, {}, 8).SetColor({ 0,255,0,127 });
#endif
	}
	void Bullet::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
#ifdef ENABLE_DEBUG_DRAW
		ring.SetPosition(p);
#endif
	}

	/*************************************************************************/
	// monster
	/*************************************************************************/

	void Monster::Init(Scene* const& scene) {
		this->scene = scene;
		this->indexAtMonsters = scene->monsters.size() - 1;

		this->radius = scene->rnd.Next(4, 32);
		this->speed = 1;
		auto r = scene->rnd.Next<float>(0, M_PI * 2);
		this->pos = xx::XY{ std::sin(r), std::cos(r) } * 1000;	// screen edge
		this->inc = (scene->player->pos - this->pos).MakeNormalize();
		this->life = 2000;

		this->SGCInit(&scene->sgMonsters);
		this->SGCSetPos(this->pos.As<int32_t>() + 6400);
		this->SGCAdd();

		DrawInit();
	}

	int Monster::Update() {
		// todo: hit player check

		this->SGCSetPos(this->pos.As<int32_t>() + 6400);
		this->SGCUpdate();

		pos += inc * speed;	// todo: AI move
		return --life;
	}

	Monster::~Monster() {
		if (_sgc) {
			SGCRemove();
		}
	}

	void Monster::DrawInit() {
		body.SetTexture(scene->tex).SetColor({ 0,0,255,127 }).SetRotate(std::atan2(inc.y, inc.x) + M_PI / 2).SetScale(radius  * 2 / body.Size().x);
#ifdef ENABLE_DEBUG_DRAW
		ring.FillCirclePoints({}, radius, {}, 8).SetColor({ 0,255,0,127 });
#endif
	}

	void Monster::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
#ifdef ENABLE_DEBUG_DRAW
		ring.SetPosition(p);
#endif
	}

	/*************************************************************************/
	// hp label
	/*************************************************************************/

	void HPLabel::Init(Scene* const& scene, xx::XY const& pos, int64_t const& hp) {
		this->scene = scene;
		this->pos = pos;
		this->life = 60;
		this->txt = std::to_string(hp);
		this->inc = { 0, -1 };
		DrawInit();
	}

	int HPLabel::Update() {
		pos += inc;
		return --life;
	}

	void HPLabel::DrawInit() {
		lbl.SetText(scene->fnt, txt);
	}

	void HPLabel::DrawCommit() {
		lbl.SetPosition(pos.MakeFlipY());
	}

	/*************************************************************************/
	// looper
	/*************************************************************************/

	void Looper::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ShootGameWithIndex::Looper::Init" << std::endl;
		scene.Init();
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
