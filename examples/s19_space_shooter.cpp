#include "xx2d_pch.h"
#include "game_looper.h"
#include "s19_space_shooter.h"
#include "xx_coro_simple.h"

namespace SpaceShooter {

	/***********************************************************************/

	void Monster::Init(Scene* owner_, xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_, float const& speed_, xx::RGBA8 const& color_) {
		// store args
		owner = owner_;
		originalPos = pos_;
		mpc = std::move(mpc_);
		speed = speed_;
		color = color_;

		// init pos
		auto mp = mpc->Move(0);
		radians = mp->radians;
		pos = originalPos + mp->pos;

		// init draw
		body.SetFrame(owner->framesMonster[0]).SetColor(color).SetScale(owner->scale);
	}
	int Monster::Update() {
		movedDistance += speed;
		auto mp = mpc->Move(movedDistance);
		if (!mp) return 1;
		radians = mp->radians;
		pos = originalPos + mp->pos;
		return 0;
	}
	void Monster::Draw() {
		body.SetPosition(pos).SetRotate(-radians + float(M_PI / 2)).Draw();
	}

	/***********************************************************************/

	void Bullet::Init(Scene* owner, xx::XY const& pos) {	// todo: more args
		this->owner = owner;
		body.SetFrame(owner->framesBullet[0]).SetScale(owner->scale);
		this->pos = pos;
		speed = 1.f * owner->scale;
		inc = { 0, speed };
		life = 60 * 5;
	}
	int Bullet::Update() {
		// todo: collision detection
		pos += inc;
		return --life <= 0 ? 1 : 0;
	}
	void Bullet::Draw() {
		body.SetPosition(pos).Draw();
	}

	/***********************************************************************/

	void Plane::Init(Scene* owner) {
		this->owner = owner;
		body.SetFrame(owner->framesPlane[1]).SetScale(owner->scale);
		pos = {};
		inc = {};
		speed = 0.5f * owner->scale;
		frame = 2;	// 1 ~ 3
	}
	int Plane::Update() {

		// move body
		//if (xx::engine.Pressed(xx::Mbtns::Left) || xx::engine.Pressed(xx::Mbtns::Right)) {
		auto x = pos.x;	// bak for change frame compare
		auto& mp = xx::engine.mousePosition;
		auto d = mp - pos;
		auto dd = d.x * d.x + d.y * d.y;
		if (speed * speed > dd) {
			inc = {};
			pos = mp;
		} else {
			inc = d.As<float>() / std::sqrt(float(dd)) * speed;
			pos += inc;
		}
		//}

		// change frame display
		constexpr float finc = 0.1f, fmin = 1.f, fmid = 2.f, fmax = 3.f;
		if (x < pos.x) {
			frame += finc;
			if (frame > fmax) {
				frame = fmax;
			}
		} else if (x > pos.x) {
			frame -= finc;
			if (frame < fmin) {
				frame = fmin;
			}
		} else {
			if (frame > fmid) {
				frame -= finc;
				if (frame < fmid) {
					frame = fmid;
				}
			} else if (frame < fmid) {
				frame += finc;
				if (frame > fmid) {
					frame = fmid;
				}
			}
		}
		auto fidx = int(frame + 0.5f) - 1;
		body.SetFrame(owner->framesPlane[fidx]);


		// auto shoot
		if (fireCD < owner->frameCounter) {
			fireCD = owner->frameCounter + 20;
			auto&& b = owner->bullets.emplace_back().Emplace();
			b->Init(owner, pos + xx::XY{ 0, 8 * owner->scale });		// todo: more style
		}

		return 0;
	}
	void Plane::Draw() {
		body.SetPosition(pos).Draw();
	}


	/***********************************************************************/

	void Space::Init(Scene* owner) {
		this->owner = owner;
		body.SetFrame(owner->framesBackground[0]).SetScale(owner->bgScale);
		yInc = 0.1f * owner->bgScale;
		ySize = body.texRectH * owner->bgScale;
	}
	int Space::Update() {
		yPos += yInc;
		if (yPos > ySize) {
			yPos -= ySize;
		}
		return 0;
	}
	void Space::Draw() {
		body.SetPositionY(-yPos).Draw();
		body.SetPositionY(-yPos + ySize).Draw();
	}


	/***********************************************************************/

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "SpaceShooter::Scene::Init" << std::endl;

		// res preload
		// p1 ~ 3, n0 ~ 9, b1 ~ 9, r1 ~ 2, m1
		tp.Fill("res/space_shooter/space_shooter.plist");

		// array -> map for easy search
		std::unordered_map<std::string_view, xx::Shared<xx::Frame>> fs;
		for (auto& f : tp.frames) {
			fs[std::string_view(f->key)] = f;
		}

		// begin fill res containers
		char buf[3];

		buf[0] = 'p';
		for (char c = '1'; c <= '3'; ++c) {
			buf[1] = c;
			framesPlane.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'n';
		for (char c = '0'; c <= '9'; ++c) {
			buf[1] = c;
			framesNumber.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'b';
		for (char c = '1'; c <= '9'; ++c) {
			buf[1] = c;
			framesBullet.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'r';
		for (char c = '1'; c <= '2'; ++c) {
			buf[1] = c;
			framesRocket.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'm';
		for (char c = '1'; c <= '1'; ++c) {
			buf[1] = c;
			framesMonster.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'b'; buf[1] = 'g';
		for (char c = '1'; c <= '1'; ++c) {
			buf[2] = c;
			framesBackground.push_back(fs[std::string_view(buf, 3)]);
		}

		// begin fill move paths
		{
			auto mp = xx::Make<xx::MovePath>();
			std::vector<xx::CurvePoint> cps;
			cps.emplace_back(xx::CurvePoint{ { 0, 0 }, 0.2f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 1500, 0 }, 0.2f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 1500, -200 }, 0.2f, 100 });
			cps.emplace_back(xx::CurvePoint{ { 0, -200 }, 0.2f, 100 });
			mp->FillCurve(true, cps);
			mpcsMonster.emplace_back(xx::Make<xx::MovePathCache>())->Init(mp, 1);
		}

		// set env
		bgScale = xx::engine.w / framesBackground[0]->spriteSourceSize.x;
		scale = bgScale / 4;

		// init all
		space.Init(this);
		plane.Init(this);
		// ...

		coros.Add(SceneLogic());
	}

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 120) {
			timePool -= 1.f / 120;

			// step frame counter
			++frameCounter;

			// generate monsters
			coros();

			// move bg
			space.Update();

			// move player's plane
			plane.Update();

			// move bullets & collision detection
			for (auto i = (ptrdiff_t)bullets.size() - 1; i >= 0; --i) {
				auto& o = bullets[i];
				if (o->Update()) {
					o = bullets.back();
					bullets.pop_back();
				}
			}

			// move monster's bullets & collision detection
			// todo

			// move monsters & collision detection
			for (auto i = (ptrdiff_t)monsters.size() - 1; i >= 0; --i) {
				auto& m = monsters[i];
				if (m->Update()) {
					EraseMonster(m);
				}
			}

			// ...
		}

		space.Draw();
		for (auto& o : monsters) o->Draw();
		plane.Draw();
		for (auto& o : bullets) o->Draw();
		// ...

		return 0;
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

	xx::Coro Scene::SceneLogic() {
		while (true) {
			{
				auto&& mpc = mpcsMonster[0];
				for (size_t i = 0; i < 10; i++) {
					auto&& m = AddMonster();
					m->Init(this, { -1000, 0 }, mpc, 2.f, { 255,255,255,255 });
					CoSleep(1s);
				}
			}
			CoSleep(5s);
		}
	}
}
