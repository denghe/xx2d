#include "main.h"
#include "s22_action.h"

namespace ActionTest {

	xx::Coro Foo::Action_Shake() {
		CoYield;
		float r1 = -0.07f, r2 = 0.07f, step = 0.01f;
		while (true) {
			for (radians = r1; radians < r2; radians += 0.01f) {
				CoYield;
			}
			for (radians = r2; radians >= r1; radians -= 0.01f) {
				CoYield;
			}
		}
	}

	xx::Coro Foo::Action_MoveTo(xx::XY tar) {
		CoYield;
		while (true) {
			auto v = tar - pos;
			if (v.x * v.x + v.y * v.y <= speed * speed) {
				pos = tar;
				break;
			} else {
				pos += v.MakeNormalize() * speed;
			}
			CoYield;
		}
	}

	xx::Coro Foo::Action_Shake_MoveTo(xx::XY tar) {
		CoYield;
		xx::Coros cs;
		cs.Add(Action_Shake());
		cs.Add(Action_MoveTo(tar));
		while (cs()) CoYield;
	}


	void Foo::Init(Scene* scene_) {
		scene = scene_;
		pos = xx::engine.mousePosition;
		speed = 2.f;
		action.emplace(Action_Shake());
		DrawInit();
	}

	int Foo::Update() {
		if (action.has_value()) {
			if (action->Resume()) {
				action.reset();
			}
		}
		return 0;
	}

	void Foo::DrawInit() {
		body.SetTexture(scene->tex);
	}
	void Foo::Draw() {
		body.SetPosition(pos).SetRotate(radians).Draw();
	}

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "ActionTest::Scene::Init" << std::endl;

		tex = xx::engine.LoadSharedTexture("res/zazaka.pkm");
		foos.Reserve(32768);
	}

	int Scene::Update() {
		
		// fixed frame rate logic
		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			if (xx::engine.Pressed(xx::Mbtns::Left)) {
				(new (&foos.Add()) Foo())->Init(this);
			}
			if (xx::engine.Pressed(xx::Mbtns::Right)) {
				for (auto idx = foos.head; idx != -1; idx = foos.Next(idx)) {
					auto& foo = foos[idx];
					foo.action.emplace(foo.Action_Shake_MoveTo(xx::engine.mousePosition));
				}
			}

			int prev = -1, next{};
			for (auto idx = foos.head; idx != -1;) {
				if (foos[idx].Update()) {
					next = foos.Remove(idx, prev);
				} else {
					next = foos.Next(idx);
					prev = idx;
				}
				idx = next;
			}
		}

		// draw
		for (auto idx = foos.head; idx != -1; idx = foos.Next(idx)) {
			foos[idx].Draw();
		}

		// draw tips text
		xx::SimpleLabel().SetPosition(xx::engine.ninePoints[8].MakeAdd(0, -32))
			.SetText(looper->fontBase, "mouse left btn: create    right btn: set action")
			.Draw();

		return 0;
	}
}
