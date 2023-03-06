#include "xx2d_pch.h"
#include "game_looper.h"
#include "s19_space_shooter.h"

namespace SpaceShooter {

	/***********************************************************************/

	void Bullet::Init(Scene* owner, xx::XY const& pos) {
		this->owner = owner;
		body.SetFrame(owner->framesBullet[0]).SetScale(owner->scale);
		this->pos = pos;
		speed = 3.f * owner->scale;
		inc = { 0, speed };
		life = 60 * 5;
	}
	int Bullet::Update() {
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
		speed = 5.f * owner->scale;
	}
	int Plane::Update() {

		if (xx::engine.Pressed(xx::Mbtns::Left) || xx::engine.Pressed(xx::Mbtns::Right)) {
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
			// todo: left, right check set frame
		}

		auto&& b = owner->bullets.emplace_back().Emplace();
		b->Init(owner, pos + xx::XY{ 0, 8 * owner->scale });		// todo: more style

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

		// set env
		bgScale = xx::engine.w / framesBackground[0]->spriteSourceSize.x;
		scale = bgScale / 4;

		// init all
		space.Init(this);
		plane.Init(this);
		// ...
	}

	int Scene::Update() {

		timePool += xx::engine.delta;
		while (timePool >= 1.f / 120) {
			timePool -= 1.f / 120;

			space.Update();
			plane.Update();
			for (auto i = (ptrdiff_t)bullets.size() - 1; i >= 0; --i) {
				auto& o = bullets[i];
				if (o->Update()) {
					o = bullets.back();
					bullets.pop_back();
				}
			}
			// ...
		}

		space.Draw();
		plane.Draw();
		for (auto& o : bullets) o->Draw();
		// ...

		return 0;
	}

}
