#include "pch.h"
#include "logic.h"
#include "logic3.h"

void Logic3::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic3 Init( test button )" << std::endl;

	meListener.Init(eg, xx::Mbtns::Left);

	auto x = eg->ninePoints[1].x;
	auto y = eg->ninePoints[1].y;
	for (size_t i = 0; i < 35; i++) {
		for (size_t j = 0; j < 63; j++) {
			auto&& b = btns.emplace_back();
			b.Init(eg, { x + i * 54 + 54 / 2, y + j * 17 + 17 / 2 }, { 52, 15 }, xx::ToString(i, '_', j), 12);
		}
	}
}

int Logic3::Update() {

	// handle mouse event
	meListener.Update();
	auto&& iter = btns.begin();
	while (meListener.eventId && iter != btns.end()) {
		meListener.Dispatch(&*iter++);
	}

	// layered draw for auto batch
	for (auto&& b : btns) {
		b.content.Draw(eg);
	}
	for (auto&& b : btns) {
		b.border.Draw(eg);
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
	if (Inside(eg->mousePosition)) {
		std::cout << "btn clicked. txt = " << txt << std::endl;
	}
}

void Button::Init(Logic* eg_, xx::XY const& pos, xx::Size const& borderSize, std::string_view const& txt_, float const& fontSize) {
	eg = eg_;
	txt = txt_;
	auto hw = borderSize.w / 2;
	auto hh = borderSize.h / 2;
	leftBottom = { pos.x - hw, pos.y - hh };
	rightTop = { pos.x + hw, pos.y + hh };
	border.SetPoints() = { {-hw,-hh},{-hw,hh},{hw,hh},{hw,-hh},{-hw,-hh} };
	border.SetPositon(pos);
	border.Commit();
	content.SetText(eg->fnt1, txt, fontSize);
	content.SetPositon(pos);
	content.Commit();
}

bool Button::Inside(xx::XY const& p) {
	return p.x >= leftBottom.x && p.x <= rightTop.x
		&& p.y >= leftBottom.y && p.y <= rightTop.y;
}
