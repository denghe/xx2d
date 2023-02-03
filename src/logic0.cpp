#include "pch.h"
#include "logic.h"
#include "logic0.h"
#include "logic1.h"
#include "logic2.h"
#include "logic3.h"
#include "logic4.h"
#include "logic5.h"
#include "logic6.h"

void Logic0::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic0 Init( main menu )" << std::endl;

	meListener.Init(eg, Mbtns::Left);
	menus.emplace_back().Init(eg, { -300, 100 }, "logic1", 64);
	menus.emplace_back().Init(eg, { 0, 100 }, "logic2", 64);
	menus.emplace_back().Init(eg, { 300, 100 }, "logic3", 64);

	menus.emplace_back().Init(eg, { -300, -100 }, "logic4", 64);
	menus.emplace_back().Init(eg, { 0, -100 }, "logic5", 64);
	menus.emplace_back().Init(eg, { 300, -100 }, "logic6", 64);

	eg->extraInfo.clear();
}

int Logic0::Update() {

	// handle mouse event
	meListener.Update();
	auto&& iter = menus.begin();
	while (meListener.eventId && iter != menus.end()) {
		meListener.Dispatch(&*iter++);
	}

	for (auto&& m : menus) {
		m.content.Draw(eg);
	}

	return 0;
}

bool Menu::HandleMouseDown(MenuMouseEventListener& L) {
	return Inside(L.downPos);
}

int Menu::HandleMouseMove(MenuMouseEventListener& L) {
	return 0;
}

void Menu::HandleMouseUp(MenuMouseEventListener& L) {
	if (Inside(eg->mousePosition)) {
		if (txt == "logic1"sv) {
			eg->DelaySwitchTo<Logic1>();
		} else if (txt == "logic2"sv) {
			eg->DelaySwitchTo<Logic2>();
		} else if (txt == "logic3"sv) {
			eg->DelaySwitchTo<Logic3>();
		} else if (txt == "logic4"sv) {
			eg->DelaySwitchTo<Logic4>();
		} else if (txt == "logic5"sv) {
			eg->DelaySwitchTo<Logic5>();
		} else if (txt == "logic6"sv) {
			eg->DelaySwitchTo<Logic6>();
		} else {
			throw std::logic_error("unhandled menu");
		}
	}
}

void Menu::Init(Logic* eg_, XY const& pos, std::string_view const& txt_, float const& fontSize) {
	eg = eg_;
	txt = txt_;
	content.SetText(eg->fnt1, txt, fontSize);
	content.SetPositon(pos);
	content.Commit();
	auto hw = content.maxSize.w / 2;
	auto hh = content.maxSize.h / 2;
	leftBottom = { pos.x - hw, pos.y - hh };
	rightTop = { pos.x + hw, pos.y + hh };
}

bool Menu::Inside(XY const& p) {
	return p.x >= leftBottom.x && p.x <= rightTop.x
		&& p.y >= leftBottom.y && p.y <= rightTop.y;
}
