#include "pch.h"
#include "logic.h"
#include "logic11.h"

namespace SG {

	/*************************************************************************/
	// scene
	/*************************************************************************/

	void Scene::Init() {
		tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
		player.Emplace()->Init(this);
	}

	void Scene::Update() {

		// update all monsters
		// todo

		player->Update();
	}

	void Scene::Draw() {
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

		// draw: sort by y
		std::sort(tmp.begin(), tmp.end(), [](xx::Sprite* const& a, xx::Sprite* const& b) {
			return a->pos.y > b->pos.y;
			});
		for (auto& q : tmp) {
			q->Draw();
		}
		tmp.clear();

		// debug
		player->ring.Draw();
		for (auto& b : player->bullets) {
			b->ring.Draw();
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
			v = (mpos - pos).Normalize();
			bullets.emplace_back().Emplace()->Init(scene, this, v, 1000);
		}
	}

	void Player::DrawInit() {
		body.SetTexture(scene->tex).SetAnchor({ 0.5, 0.2 }).SetScale(4);
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

	void Bullet::Init(Scene* scene, Player* owner, xx::XY const& inc, int const& life) {
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
		pos += inc * speed;
		return --life;
	}

	void Bullet::DrawInit() {
		body.SetTexture(scene->tex).SetColor({ 255,0,0,127 }).SetRotate(std::atan2(inc.y, inc.x) + M_PI/2);
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

	void Monster::Init(Scene* scene) {
		// todo
		DrawInit();
	}

	void Monster::Update() {
		// todo
	}

	void Monster::DrawInit() {
		// todo
	}

	void Monster::DrawCommit() {
		auto p = pos.MakeFlipY();
		body.SetPosition(p);
		ring.SetPosition(p);
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
