#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic8;

struct SpriteNode {
	xx::Sprite sprite;
	std::vector<xx::Shared<SpriteNode>> children;
};

struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	SpriteNode node;
	xx::LineStrip ls;
};
























//xx::LineStrip border, center;
