#pragma once
#include "pch.h"
#include "logic_base.h"

struct Menu;
using MenuMouseEventListener = MouseEventListener<Menu*>;

struct Menu {
	bool HandleMouseDown(MenuMouseEventListener& L);
	int HandleMouseMove(MenuMouseEventListener& L);
	void HandleMouseUp(MenuMouseEventListener& L);
	void Init(Logic* eg, XY const& pos, std::string_view const& txt, float const& fontSize);

	Logic* eg{};
	XY leftBottom, rightTop;	// bounding box
	std::string txt;

	bool Inside(XY const& point);	// bounding box contains point?

	Label content;
};

struct Logic0 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<Menu> menus;
	MenuMouseEventListener meListener;
};
