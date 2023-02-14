#include "pch.h"
#include "logic.h"
#include "logic11.h"

namespace SG {

	/*************************************************************************/
	// scene
	/*************************************************************************/

	void Scene::Init() {
		fnt.Load("res/font2/basechars.fnt");
		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
		player.Emplace()->Init(this);

		running = true;
		coros.Add([](Scene* scene)->xx::Coro {
			while (scene->running) {
				for (size_t i = 0; i < 5; ++i) {
					scene->monsters.emplace_back().Emplace()->Init(scene);
				}
				CoYield;
			}
		}(this));
	}

	void Scene::Update() {

		// call scene logic
		coros();

		/*********************************************/
		// move monsters

		for (auto i = (ptrdiff_t)monsters.size() - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (!m->Update()) {
				m = monsters.back();
				monsters.pop_back();
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
		// logic data -> display component
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
		std::sort(tmp.begin(), tmp.end(), [](xx::Sprite* const& a, xx::Sprite* const& b) {
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

		// draw debug rings
		player->ring.Draw();
		for (auto& b : player->bullets) {
			b->ring.Draw();
		}
		for (auto& m : monsters) {
			m->ring.Draw();
		}
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
			for (size_t i = 1; i <= 5; ++i) {
				auto r1 = r + 0.1f * (float)i;
				bullets.emplace_back().Emplace()->Init(scene, this, { std::sin(r1), std::cos(r1) }, 1000);
				auto r2 = r - 0.1f * (float)i;
				bullets.emplace_back().Emplace()->Init(scene, this, { std::sin(r2), std::cos(r2) }, 1000);
			}
		}
	}

	void Player::DrawInit() {
		body.SetTexture(scene->tex).SetAnchor({ 0.5, 0.25 }).SetScale(radius * 3 / body.Size().x);
		ring.FillCirclePoints({}, radius, {}, 16).SetColor({ 255,0,0,127 });
	}
	void Player::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
		ring.SetPosition(p);
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

		// hit monster check
		auto& ms = scene->monsters;
		for (auto& m : ms) {
			auto d = m->pos - pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (dd < rr) {
				scene->labels.emplace_back().Emplace()->Init(scene, m->pos, m->radius);	// pop up damage hp
				m = ms.back();
				ms.pop_back();
				return 0;
			}
		}

		pos += inc * speed;
		return --life;
	}

	void Bullet::DrawInit() {
		body.SetTexture(scene->tex).SetColor({ 255,0,0,127 }).SetRotate(std::atan2(inc.y, inc.x) + M_PI / 2);
		ring.FillCirclePoints({}, radius, {}, 8).SetColor({ 0,255,0,127 });
	}
	void Bullet::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
		ring.SetPosition(p);
	}

	/*************************************************************************/
	// monster
	/*************************************************************************/

	void Monster::Init(Scene* const& scene) {
		this->scene = scene;

		this->radius = scene->rnd.Next(4, 32);
		this->speed = 1;
		auto r = scene->rnd.Next<float>(0, M_PI * 2);
		this->pos = xx::XY{ std::sin(r), std::cos(r) } * 1000;	// screen edge
		this->inc = (scene->player->pos - this->pos).MakeNormalize();
		this->life = 2000;

		DrawInit();
	}

	int Monster::Update() {
		// todo: hit player check

		pos += inc * speed;	// todo: AI move
		return --life;
	}

	void Monster::DrawInit() {
		body.SetTexture(scene->tex).SetColor({ 0,0,255,127 }).SetRotate(std::atan2(inc.y, inc.x) + M_PI / 2).SetScale(radius  * 2 / body.Size().x);
		ring.FillCirclePoints({}, radius, {}, 8).SetColor({ 0,255,0,127 });
	}

	void Monster::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
		ring.SetPosition(p);
	}

	/*************************************************************************/
	// hp label
	/*************************************************************************/

	void HPLabel::Init(Scene* const& scene, xx::XY const& pos, int64_t const& hp) {
		this->scene = scene;
		this->pos = pos;
		this->life = 30;
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
}

/*************************************************************************/
// logic
/*************************************************************************/

void Logic11::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic11 Init( shooter game )" << std::endl;
	scene.Init();
}

int Logic11::Update() {
	timePool += xx::engine.delta;
	auto timePoolBak = timePool;
	while (timePool >= 1.f / 120) {
		timePool -= 1.f / 120;

		scene.Update();
	}
	scene.Draw();
	return 0;
}
