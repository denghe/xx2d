#pragma once
#include "pch.h"
#include "logic_base.h"

struct Button;
using ButtonMouseEventListener = xx::MouseEventListener<Button*>;

struct Button {
	bool HandleMouseDown(ButtonMouseEventListener& L);
	int HandleMouseMove(ButtonMouseEventListener& L);
	void HandleMouseUp(ButtonMouseEventListener& L);

	void Init(Logic* const& logic, xx::XY const& pos, xx::Size const& borderSize, std::string_view const& txt, float const& fontSize);

	Logic* logic;
	xx::XY leftBottom{}, rightTop{};	// bounding box
	std::string txt;

	bool Inside(xx::XY const& point);	// bounding box contains point?

	xx::LineStrip border;
	xx::Label content;
};

struct Logic3 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	std::vector<Button> btns;
	ButtonMouseEventListener meListener;
};
