#include "xx2d_pch.h"
#include "game_looper.h"
#include "s19_space_shooter.h"
#include "xx_coro_simple.h"

namespace SpaceShooter {

	/***********************************************************************/
	/***********************************************************************/

	void Score::Init(Scene* const& owner_) {
		owner = owner_;
		step = 1;
	}
	void Score::Add(int64_t const& offset) {
		to += offset;
		step = (to - from) / 120 * 2;
		if (step < 1) {
			step = 1;
		}
	}
	void Score::Update() {
		if (from < to) {
			from += step;
			if (from > to) {
				from = to;
			}
		}
	}
	void Score::Draw() {
		auto s = std::to_string(from);
		auto siz = s.size();
		auto& fs = owner->framesNumber;
		auto wh = fs[0]->spriteSourceSize * owner->scale;
		auto origin = xx::engine.ninePoints[8];
		xx::XY xy{ origin.x - wh.x * siz / 2, origin.y - 10 };
		xx::Quad q;
		q.SetScale(owner->scale).SetAnchor({ 0, 1 });
		for (auto& c : s) {
			q.SetFrame(fs[c - '0']).SetPosition(xy).Draw();
			xy.x += wh.x;
		}
	}

	/***********************************************************************/
	/***********************************************************************/

	void LabelEffect::Init(Scene* const& owner_, xx::XY const& pos_, std::string_view const& txt_) {
		owner = owner_;
		pos = pos_;

		avaliableFrameNumber = owner->frameNumber + 100;
		inc = { 0, 0.5 };

		body.SetText(owner->looper->fnt1, txt_, 64);
	}
	bool LabelEffect::Update() {
		pos += inc;
		return avaliableFrameNumber < owner->frameNumber;
	}
	void LabelEffect::Draw() {
		body.SetPosition(pos).Draw();
	}

	/***********************************************************************/
	/***********************************************************************/

	void MonsterBase::Init1(Scene* owner_, float const& speed_, xx::RGBA8 const& color_, Listener_s<MonsterBase> deathListener_) {
		assert(!owner);
		owner = owner_;
		speed = speed_;
		color = color_;
		deathListener = std::move(deathListener_);
		frameStep = 0.1f;
	}
	void MonsterBase::UpdateFrameIndex() {
		frameIndex += frameStep;
		if (frameIndex >= frames->size() || frameIndex < 0) {
			frameStep = -frameStep;
			frameIndex += frameStep;
		}
	}
	bool MonsterBase::Hit(int64_t const& damage) {
		hp -= damage;
		if (hp <= 0) {
			owner->score.Add(100);
			// todo: make explode effect ?
			if (deathListener) {
				(*deathListener)(this);
			}
			return true;
		}
		hitEffectExpireFrameNumber = owner->frameNumber + 10;
		return false;
	}
	void MonsterBase::Draw() {
		body.SetPosition(pos).SetRotate(-radians + float(M_PI / 2)).SetFrame((*frames)[frameIndex])
			.SetColor(hitEffectExpireFrameNumber > owner->frameNumber ? xx::RGBA8{ 255,0,0,255 } : color)
			.Draw();
	}

	/***********************************************************************/
	/***********************************************************************/

	void Monster::Init2(xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_) {
		assert(owner);
		originalPos = pos_;
		mpc = std::move(mpc_);
		frames = &owner->framesMonster1;
		radius = 7 * owner->scale;

		// init
		auto mp = mpc->Move(0);
		radians = mp->radians;
		pos = originalPos + mp->pos;
		hp = 20;

		// init draw
		body.SetScale(owner->scale);
	}
	bool Monster::Update() {
		movedDistance += speed;
		auto mp = mpc->Move(movedDistance);
		if (!mp) return true;

		radians = mp->radians;
		pos = originalPos + mp->pos;
		UpdateFrameIndex();
		return false;
	}

	/***********************************************************************/
	/***********************************************************************/

	void Monster2::Init2(xx::XY const& pos_, float const& radians_) {
		assert(owner);
		// store args
		pos = pos_;
		radians = radians_;
		frames = &owner->framesMonster2;
		radius = 2 * owner->scale;

		// init
		inc = { std::cos(radians),std::sin(radians) };
		inc *= speed;
		avaliableFrameNumber = owner->frameNumber + 120 * 10;
		hp = 20;

		// init draw
		body.SetScale(owner->scale / 3);
	}
	bool Monster2::Update() {
		pos += inc;
		UpdateFrameIndex();
		return avaliableFrameNumber < owner->frameNumber;
	}

	/***********************************************************************/
	/***********************************************************************/

	void Bullet::Init(Scene* owner_, xx::XY const& pos_, int64_t const& damage_) {
		owner = owner_;
		pos = pos_;
		damage = damage_;

		speed = 1.f * owner->scale;
		inc = { 0, speed };
		avaliableFrameNumber = owner->frameNumber + 120 * 2;
		radius = 2 * owner->scale;

		body.SetFrame(owner->framesBullet[0]).SetScale(owner->scale);
	}
	bool Bullet::Update() {

		// collision detection
		for (auto& m : owner->monsters) {
			auto d = m->pos - pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (dd < rr) {
				if (m->Hit(damage)) {
					owner->EraseMonster(m);	// can't access 'm' after this line
				}
				return true;
			}
		}

		pos += inc;
		return avaliableFrameNumber < owner->frameNumber;
	}
	void Bullet::Draw() {
		body.SetPosition(pos).Draw();
	}

	/***********************************************************************/
	/***********************************************************************/

	void Plane::Init(Scene* owner, int64_t const& invincibleTime) {
		this->owner = owner;
		body.SetFrame(owner->framesPlane[1]).SetScale(owner->scale);
		pos = {};
		inc = {};
		speed = 0.5f * owner->scale;
		frame = 2;	// 1 ~ 3
		radius = body.texRectH / 2;
		invincibleFrameNumber = owner->frameNumber + invincibleTime;
	}
	bool Plane::Update() {

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
		owner->lastPlanePos = pos;
		//}

		if (invincibleFrameNumber <= owner->frameNumber) {
			// collision detection
			for (auto& m : owner->monsters) {
				auto d = m->pos - pos;
				auto rr = (m->radius + radius) * (m->radius + radius);
				auto dd = d.x * d.x + d.y * d.y;
				if (dd < rr) {
					// todo: show effect?
					return true;
				}
			}
		}

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
		if (fireCD < owner->frameNumber) {
			fireCD = owner->frameNumber + 20;
			auto&& b = owner->bullets.emplace_back().Emplace();
			b->Init(owner, pos + xx::XY{ 0, 8 * owner->scale }, 10);		// todo: more style
		}

		return false;
	}
	void Plane::Draw() {
		body.SetPosition(pos).SetColor(invincibleFrameNumber > owner->frameNumber ? xx::RGBA8{255,255,255,50} : xx::RGBA8{255,255,255,255}).Draw();
	}


	/***********************************************************************/
	/***********************************************************************/

	void Space::Init(Scene* owner) {
		this->owner = owner;
		body.SetFrame(owner->framesBackground[0]).SetScale(owner->bgScale);
		yInc = 0.1f * owner->bgScale;
		ySize = body.texRectH * owner->bgScale;
	}
	void Space::Update() {
		yPos += yInc;
		if (yPos > ySize) {
			yPos -= ySize;
		}
	}
	void Space::Draw() {
		body.SetPositionY(-yPos).Draw();
		body.SetPositionY(-yPos + ySize).Draw();
	}


	/***********************************************************************/
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

		buf[0] = 'm'; buf[1] = 'a';
		for (char c = '1'; c <= '6'; ++c) {
			buf[2] = c;
			framesMonster1.push_back(fs[std::string_view(buf, 3)]);
		}

		buf[0] = 'm'; buf[1] = 'b';
		for (char c = '1'; c <= '5'; ++c) {
			buf[2] = c;
			framesMonster2.push_back(fs[std::string_view(buf, 3)]);
		}

		buf[0] = 'm'; buf[1] = 'c';
		for (char c = '1'; c <= '4'; ++c) {
			buf[2] = c;
			framesMonster3.push_back(fs[std::string_view(buf, 3)]);
		}

		buf[0] = 'b'; buf[1] = 'g';
		for (char c = '1'; c <= '1'; ++c) {
			buf[2] = c;
			framesBackground.push_back(fs[std::string_view(buf, 3)]);
		}

		buf[0] = 'e'; buf[1] = 'b';
		for (char c = '1'; c <= '4'; ++c) {
			buf[2] = c;
			framesMonsterBullet.push_back(fs[std::string_view(buf, 3)]);
		}

		buf[0] = 'e';
		for (char c = '1'; c <= '5'; ++c) {
			buf[1] = c;
			framesEffect.push_back(fs[std::string_view(buf, 2)]);
		}

		buf[0] = 'p'; buf[1] = 'o';
		for (char c = '1'; c <= '3'; ++c) {
			buf[2] = c;
			framesStuff.push_back(fs[std::string_view(buf, 3)]);
		}
		framesStuff.push_back(fs["ph"sv]);
		framesStuff.push_back(fs["ps"sv]);
		framesStuff.push_back(fs["pc"sv]);

		framesText.push_back(fs["tstart"sv]);
		framesText.push_back(fs["tgameover"sv]);

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
		score.Init(this);
		plane.Emplace()->Init(this);
		// ...

		// run script
		coros.Add(SceneLogic());
	}

	/***********************************************************************/

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 120) {
			timePool -= 1.f / 120;

			// step frame counter
			++frameNumber;

			// generate monsters
			coros();

			// move bg
			space.Update();

			// move player's plane
			if (plane && plane->Update()) {
				// todo: death effect?
				plane.Reset();
				coros.Add(SceneLogic_PlaneReborn());
			}

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
				auto& o = monsters[i];
				if (o->Update()) {
					EraseMonster(o);
				}
			}

			// move labels
			for (auto i = (ptrdiff_t)labels.size() - 1; i >= 0; --i) {
				auto& o = labels[i];
				if (o->Update()) {
					o = labels.back();
					labels.pop_back();
				}
			}

			// refresh score
			score.Update();

			// ...
		}

		space.Draw();
		for (auto& o : monsters) o->Draw();
		if (plane) plane->Draw();
		for (auto& o : bullets) o->Draw();
		for (auto& o : labels) o->Draw();
		score.Draw();
		// ...

		return 0;
	}

	/***********************************************************************/

	void Scene::AddMonster(MonsterBase* m) {
		m->indexAtOwnerMonsters = monsters.size();
		monsters.emplace_back(m);
	}

	void Scene::EraseMonster(MonsterBase* m) {
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

	void Scene::ShowBonusEffect(xx::XY const& pos, int64_t const& value) {
		labels.emplace_back().Emplace()->Init(this, pos, xx::ToString("+", value));
	}

	/***********************************************************************/

	xx::Coro Scene::SceneLogic() {
		while (true) {
			coros.Add(SceneLogic_CreateMonsterTeam(5, 2000));
			CoSleep(5s);
			{
				int n1 = 120 * 5, n2 = 50;
				for (int i = 0; i < n1; i++) {
					for (int j = 0; j < n2; j++) {
						auto radians = rnd.Next<float>(0, M_PI);
						xx::XY v{ std::cos(radians),std::sin(radians) };
						auto bornPos = v * (xx::engine.hw + 200);
						auto d = lastPlanePos - bornPos;
						radians = std::atan2(d.y, d.x);
						auto m = xx::Make<Monster2>();
						m->Init1(this, 2.f, { 255,255,255,255 });
						m->Init2(bornPos, radians);
						AddMonster(m);
					}
					CoYield;	//CoSleep(50ms);
				}
			}
			// ...
		}
	}

	xx::Coro Scene::SceneLogic_CreateMonsterTeam(int n, int64_t bonus) {
		auto dt = xx::Make<Listener<MonsterBase>>([this, n, bonus] (MonsterBase* m) mutable {
			if (--n == 0) {
				score.Add(bonus);
				ShowBonusEffect(m->pos, bonus);
			}
		});
		auto&& mpc = mpcsMonster[0];
		for (int i = 0; i < n; i++) {
			auto m = xx::Make<Monster>();
			m->Init1(this, 2.f, { 255,255,255,255 }, dt);
			m->Init2({ -1000, 300 }, mpc);
			AddMonster(m);
			CoSleep(600ms);
		}
	}

	xx::Coro Scene::SceneLogic_PlaneReborn() {
		CoSleep(3s);
		assert(!plane);
		plane.Emplace()->Init(this, 240);
	}

}
