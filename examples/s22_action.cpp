#include "main.h"
#include "s22_action.h"

namespace ActionTest {

	xx::Coro Foo::Action_Shake(float r1, float r2, float step) {
		CoYield;
		while (true) {	// repeat forever
			for (radians = r1; radians < r2; radians += step) {
				CoYield;
			}
			for (radians = r2; radians >= r1; radians -= step) {
				CoYield;
			}
		}
	}

	xx::Coro Foo::Action_MoveTo(xx::XY tar) {
		CoYield;
		while (true) {	// repeat until
			auto v = tar - pos;
			if (v.x * v.x + v.y * v.y <= speed * speed) {
				pos = tar;
				break;
			} else {
				pos += v.MakeNormalize() * speed;
			}
			CoYield;
		}
		dying = true;	// set state flag
	}

	xx::Coro Foo::Action_Shake_MoveTo(xx::XY tar) {
		CoYield;
		xx::Coros cs(2);
		cs.Add(Action_Shake(-0.07f, 0.07f, 0.03f));
		cs.Add(Action_MoveTo(tar));
		while (cs() && cs.Count() == 2) CoYield;	// wait one
		cs.Clear();
		cs.Add(Action_FadeOut(1.f / 60));
		cs.Add(Action_ScaleTo(0, 1.f / 60));
		while (cs()) CoYield;						// wait all
		dead = true;
	}

	xx::Coro Foo::Action_FadeOut(float step) {
		CoYield;
		for (; alpha >= 0; alpha -= step) {
			CoYield;
		}
		alpha = 0;
	}

	xx::Coro Foo::Action_ScaleTo(float s, float step) {
		CoYield;
		for (; scale >= s; scale -= step) {
			CoYield;
		}
		scale = s;
	}


	void Foo::Init(Scene* scene_) {
		scene = scene_;
		pos = xx::engine.mousePosition;
		speed = 2.f;
		action.emplace(Action_Shake(-0.07f, 0.07f, 0.005));
		DrawInit();
	}

	bool Foo::Update() {
		if (action.has_value()) {
			if (action->Resume()) {
				action.reset();
			}
		}
		return dead;
	}

	void Foo::DrawInit() {
		body.SetTexture(scene->tex);
	}
	void Foo::Draw() {
		body.SetPosition(pos).SetRotate(radians).SetColorA(alpha).SetScale(scale).Draw();
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
					if (foo.dying) continue;
					foo.action.emplace(foo.Action_Shake_MoveTo(xx::engine.mousePosition));
				}
			}

			foos.Foreach([](auto& foo)->bool {
				return foo.Update();
			});
		}

		// draw
		foos.Foreach([](auto& foo) {
			foo.Draw();
		});

		// draw tips text
		xx::SimpleLabel().SetPosition(xx::engine.ninePoints[8].MakeAdd(0, -32))
			.SetText(looper->fontBase, "mouse left btn: create    right btn: set action")
			.Draw();

		return 0;
	}
}
