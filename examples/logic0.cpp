#include "xx2d_pch.h"
#include "game_looper.h"
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
#include "logic13.h"
#include "logic14.h"
#include "logic15.h"

void Logic0::Init(GameLooper* looper) {
	this->looper = looper;

	std::cout << "Logic0 Init( main menu )" << std::endl;

	meListener.Init(xx::Mbtns::Left);

	float x = 300;
	menus.emplace_back().Init(looper, { -500, x }, "1: tiledmap", 48);
	menus.emplace_back().Init(looper, { 0, x }, "2: circle drag", 48);
	menus.emplace_back().Init(looper, { 500, x }, "3: box button", 48);

	x -= 150;
	menus.emplace_back().Init(looper, { -500, x }, "4: space grid physics", 48);
	menus.emplace_back().Init(looper, { 0, x }, "5: space grid ab", 48);
	menus.emplace_back().Init(looper, { 500, x }, "6: circle + box physics", 48);

	x -= 150;
	menus.emplace_back().Init(looper, { -500, x }, "7: more circle + box", 48);
	menus.emplace_back().Init(looper, { 0, x }, "8: node tests", 48);
	menus.emplace_back().Init(looper, { 500, x }, "9: sprite tests", 48);

	x -= 150;
	menus.emplace_back().Init(looper, { -500, x }, "10: quad instance tests", 48);
	menus.emplace_back().Init(looper, { 0, x }, "11: shooter game", 48);
	menus.emplace_back().Init(looper, { 500, x }, "12: shooter game with grid", 48);

	x -= 150;
	menus.emplace_back().Init(looper, { -500, x }, "13: move path tests", 48);
	menus.emplace_back().Init(looper, { 0, x }, "14: scissor tests", 48);
	menus.emplace_back().Init(looper, { 500, x }, "15: render texture tests", 48);

	looper->extraInfo.clear();
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
			looper->DelaySwitchTo<Logic1>();
		} else if (txt.starts_with("2:"sv)) {
			looper->DelaySwitchTo<Logic2>();
		} else if (txt.starts_with("3:"sv)) {
			looper->DelaySwitchTo<Logic3>();
		} else if (txt.starts_with("4:"sv)) {
			looper->DelaySwitchTo<Logic4>();
		} else if (txt.starts_with("5:"sv)) {
			looper->DelaySwitchTo<Logic5>();
		} else if (txt.starts_with("6:"sv)) {
			looper->DelaySwitchTo<Logic6>();
		} else if (txt.starts_with("7:"sv)) {
			looper->DelaySwitchTo<Logic7>();
		} else if (txt.starts_with("8:"sv)) {
			looper->DelaySwitchTo<Logic8>();
		} else if (txt.starts_with("9:"sv)) {
			looper->DelaySwitchTo<Logic9>();
		} else if (txt.starts_with("10:"sv)) {
			looper->DelaySwitchTo<Logic10>();
		} else if (txt.starts_with("11:"sv)) {
			looper->DelaySwitchTo<Logic11>();
		} else if (txt.starts_with("12:"sv)) {
			looper->DelaySwitchTo<Logic12>();
		} else if (txt.starts_with("13:"sv)) {
			looper->DelaySwitchTo<Logic13>();
		} else if (txt.starts_with("14:"sv)) {
			looper->DelaySwitchTo<Logic14>();
		} else if (txt.starts_with("15:"sv)) {
			looper->DelaySwitchTo<Logic15>();
		} else {
			throw std::logic_error("unhandled menu");
		}
	}
}

void Menu::Init(GameLooper* looper, xx::XY const& pos, std::string_view const& txt_, float const& fontSize) {
	this->looper = looper;
	txt = txt_;
	content.SetText(looper->fnt1, txt, fontSize);
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
