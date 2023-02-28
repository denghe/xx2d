#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"

struct Menu;
using MenuMouseEventListener = xx::MouseEventListener<Menu*>;

struct Menu {
	bool HandleMouseDown(MenuMouseEventListener& L);
	int HandleMouseMove(MenuMouseEventListener& L);
	void HandleMouseUp(MenuMouseEventListener& L);
	void Init(GameLooper* looper, xx::XY const& pos, std::string_view const& txt, float const& fontSize);

	GameLooper* looper{};
	xx::XY leftBottom, rightTop;	// bounding box
	std::string txt;

	bool Inside(xx::XY const& point);	// bounding box contains point?

	xx::Label content;
};

struct Logic0 : LogicBase {
	void Init(GameLooper* looper) override;
	int Update() override;

	std::vector<Menu> menus;
	MenuMouseEventListener meListener;
};
