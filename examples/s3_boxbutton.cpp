#include "main.h"
#include "s3_boxbutton.h"

namespace BoxButton {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "BoxButton::Scene::Init" << std::endl;

		meListener.Init(xx::Mbtns::Left);

		auto x = xx::engine.ninePoints[1].x;
		auto y = xx::engine.ninePoints[1].y;
		for (size_t i = 0; i < 35; i++) {
			for (size_t j = 0; j < 63; j++) {
				auto&& b = btns.emplace_back();
				b.Init(looper, { x + i * 54 + 54 / 2, y + j * 17 + 17 / 2 }, { 52, 15 }, xx::ToString(i, '_', j), 12);
			}
		}
	}

	int Scene::Update() {

		// handle mouse event
		meListener.Update();
		auto&& iter = btns.begin();
		while (meListener.eventId && iter != btns.end()) {
			meListener.Dispatch(&*iter++);
		}

		// layered draw for auto batch
		for (auto&& b : btns) {
			b.content.Draw();
		}
		for (auto&& b : btns) {
			b.border.Draw();
		}

		return 0;
	}

	bool Button::HandleMouseDown(ButtonMouseEventListener& L) {
		return Inside(L.downPos);
	}

	int Button::HandleMouseMove(ButtonMouseEventListener& L) {
		return 0;
	}

	void Button::HandleMouseUp(ButtonMouseEventListener& L) {
		if (Inside(xx::engine.mousePosition)) {
			std::cout << "btn clicked. txt = " << txt << std::endl;
		}
	}

	void Button::Init(GameLooper* const& looper, xx::XY const& pos, xx::XY const& borderSize, std::string_view const& txt_, float const& fontSize) {
		this->looper = looper;
		txt = txt_;
		border.FillBoxPoints({}, borderSize)
			.SetPosition(pos);
		content.SetText(looper->fontBase, txt, fontSize)
			.SetPosition(pos);
	}

	bool Button::Inside(xx::XY const& p) {
		return p.x >= leftBottom.x && p.x <= rightTop.x
			&& p.y >= leftBottom.y && p.y <= rightTop.y;
	}

}
