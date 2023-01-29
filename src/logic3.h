#pragma once
#include "pch.h"
#include "logic_base.h"

struct Button;
using ButtonMouseEventListener = MouseEventListener<Button*>;

struct Button {
	bool HandleMouseDown(ButtonMouseEventListener& L);
	int HandleMouseMove(ButtonMouseEventListener& L);
	void HandleMouseUp(ButtonMouseEventListener& L);

	void Init(Logic* eg, XY const& pos, Size const& borderSize, std::string_view const& txt, float const& fontSize);

	Logic* eg{};
	XY leftBottom, rightTop;	// bounding box
	std::string txt;

	bool Inside(XY const& point);	// bounding box contains point?

	LineStrip border;
	Label content;
};

struct Logic3 : LogicBase {
	void Init(Logic* eg) override;
	int Update() override;

	std::vector<Button> btns;
	ButtonMouseEventListener meListener;
};
