#include "pch.h"
#include "logic.h"
#include "logic0.h"
#include "logic1.h"
#include "logic2.h"
#include "logic3.h"
#include "logic4.h"
#include "logic5.h"
#include "logic6.h"
#include "logic7.h"
#include "logic8.h"

void Logic0::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic0 Init( main menu )" << std::endl;

	meListener.Init(xx::Mbtns::Left);
	menus.emplace_back().Init(logic, { -500, 200 }, "1: tiledmap", 64);
	menus.emplace_back().Init(logic, { 0, 200 }, "2: circle drag", 64);
	menus.emplace_back().Init(logic, { 500, 200 }, "3: box button", 64);

	menus.emplace_back().Init(logic, { -500, 0 }, "4: space grid physics", 64);
	menus.emplace_back().Init(logic, { 0, 0 }, "5: space grid ab", 64);
	menus.emplace_back().Init(logic, { 500, 0 }, "6: circle + box physics", 64);

	menus.emplace_back().Init(logic, { -500, -200 }, "7: more circle + box", 64);
	menus.emplace_back().Init(logic, { 0, -200 }, "8: node tests", 64);

	logic->extraInfo.clear();
}

int Logic0::Update() {

	// handle mouse event
	meListener.Update();
	auto&& iter = menus.begin();
	while (meListener.eventId && iter != menus.end()) {
		meListener.Dispatch(&*iter++);
	}

	for (auto&& m : menus) {
		m.content.Draw();
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
	if (Inside(xx::engine.mousePosition)) {
		if (txt.starts_with("1:"sv)) {
			logic->DelaySwitchTo<Logic1>();
		} else if (txt.starts_with("2:"sv)) {
			logic->DelaySwitchTo<Logic2>();
		} else if (txt.starts_with("3:"sv)) {
			logic->DelaySwitchTo<Logic3>();
		} else if (txt.starts_with("4:"sv)) {
			logic->DelaySwitchTo<Logic4>();
		} else if (txt.starts_with("5:"sv)) {
			logic->DelaySwitchTo<Logic5>();
		} else if (txt.starts_with("6:"sv)) {
			logic->DelaySwitchTo<Logic6>();
		} else if (txt.starts_with("7:"sv)) {
			logic->DelaySwitchTo<Logic7>();
		} else if (txt.starts_with("8:"sv)) {
			logic->DelaySwitchTo<Logic8>();
		} else {
			throw std::logic_error("unhandled menu");
		}
	}
}

void Menu::Init(Logic* logic, xx::XY const& pos, std::string_view const& txt_, float const& fontSize) {
	this->logic = logic;
	txt = txt_;
	content.SetText(logic->fnt1, txt, fontSize);
	content.SetPositon(pos);
	content.Commit();
	auto hw = content.maxSize.w / 2;
	auto hh = content.maxSize.h / 2;
	leftBottom = { pos.x - hw, pos.y - hh };
	rightTop = { pos.x + hw, pos.y + hh };
}

bool Menu::Inside(xx::XY const& p) {
	return p.x >= leftBottom.x && p.x <= rightTop.x
		&& p.y >= leftBottom.y && p.y <= rightTop.y;
}
