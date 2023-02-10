#pragma once
#include "pch.h"
#include "logic_base.h"
#include "nodes.h"

struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::SpriteNode node;
};

//xx::LineStrip ls;
//xx::LineStrip border, center;
