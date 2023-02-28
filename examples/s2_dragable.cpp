#include "xx2d_pch.h"
#include "game_looper.h"
#include "s2_dragable.h"

namespace Dragable {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "Dragable::Scene::Init" << std::endl;

		// test mouse event listener
		meListener.Init(xx::Mbtns::Left);

		// test line strip
		for (size_t i = 0; i < 10000; i++) {
			xx::XY v{ float(rand() % ((int)xx::engine.w - 30)) + 15 - xx::engine.hw, float(rand() % ((int)xx::engine.h - 30)) + 15 - xx::engine.hh };

			auto&& c = circles.emplace_back();
			c.Init(v, 15, 12);
		}
	}

	int Scene::Update() {

		// test mouse event listener
		meListener.Update();
		auto&& iter = circles.begin();
		while (meListener.eventId && iter != circles.end()) {
			meListener.Dispatch(&*iter++);
		}

		// test line strip
		for (auto&& c : circles) {
			c.Draw();
		}

		return 0;
	}

	bool DragableCircle::HandleMouseDown(DragableCircleMouseEventListener& L) {
		auto dx = pos.x - L.downPos.x;
		auto dy = pos.y - L.downPos.y;
		return dx * dx + dy * dy < radius_square;
	}

	int DragableCircle::HandleMouseMove(DragableCircleMouseEventListener& L) {
		this->SetPosition(this->pos + (xx::engine.mousePosition - L.lastPos));
		return 0;
	}

	void DragableCircle::HandleMouseUp(DragableCircleMouseEventListener& L) {
		std::cout << "L.downPos.x = " << L.downPos.x << ", L.downPos.y = " << L.downPos.y << std::endl;
	}

	void DragableCircle::Init(xx::XY const& pos, float const& radius, int const& segments) {
		this->radius_square = radius * radius;
		this->FillCirclePoints({ 0,0 }, radius, {}, segments)
			.SetColor({ 255, 255, 0, 255 })
			.SetPosition(pos);
	}

}
