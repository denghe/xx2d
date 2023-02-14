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
#include "logic9.h"
#include "logic10.h"
#include "logic11.h"
#include "logic12.h"

void Logic0::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic0 Init( main menu )" << std::endl;

	meListener.Init(xx::Mbtns::Left);
	menus.emplace_back().Init(logic, { -500, 300 }, "1: tiledmap", 48);
	menus.emplace_back().Init(logic, { 0, 300 }, "2: circle drag", 48);
	menus.emplace_back().Init(logic, { 500, 300 }, "3: box button", 48);

	menus.emplace_back().Init(logic, { -500, 100 }, "4: space grid physics", 48);
	menus.emplace_back().Init(logic, { 0, 100 }, "5: space grid ab", 48);
	menus.emplace_back().Init(logic, { 500, 100 }, "6: circle + box physics", 48);

	menus.emplace_back().Init(logic, { -500, -100 }, "7: more circle + box", 48);
	menus.emplace_back().Init(logic, { 0, -100 }, "8: node tests", 48);
	menus.emplace_back().Init(logic, { 500, -100 }, "9: sprite tests", 48);

	menus.emplace_back().Init(logic, { -500, -300 }, "10: quad instance tests", 48);
	menus.emplace_back().Init(logic, { 0, -300 }, "11: shooter game", 48);
	menus.emplace_back().Init(logic, { 500, -300 }, "12: shooter game with grid", 48);

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
		} else if (txt.starts_with("9:"sv)) {
			logic->DelaySwitchTo<Logic9>();
		} else if (txt.starts_with("10:"sv)) {
			logic->DelaySwitchTo<Logic10>();
		} else if (txt.starts_with("11:"sv)) {
			logic->DelaySwitchTo<Logic11>();
		} else if (txt.starts_with("12:"sv)) {
			logic->DelaySwitchTo<Logic12>();
		} else {
			throw std::logic_error("unhandled menu");
		}
	}
}

void Menu::Init(Logic* logic, xx::XY const& pos, std::string_view const& txt_, float const& fontSize) {
	this->logic = logic;
	txt = txt_;
	content.SetText(logic->fnt1, txt, fontSize);
	content.SetPosition(pos);
	auto hw = content.size.x / 2;
	auto hh = content.size.y / 2;
	leftBottom = { pos.x - hw, pos.y - hh };
	rightTop = { pos.x + hw, pos.y + hh };
}

bool Menu::Inside(xx::XY const& p) {
	return p.x >= leftBottom.x && p.x <= rightTop.x
		&& p.y >= leftBottom.y && p.y <= rightTop.y;
}
