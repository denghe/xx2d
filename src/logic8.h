#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic8;

struct SpriteNode {
	xx::Sprite sprite;
	xx::LineStrip border, center;
	std::vector<xx::Shared<SpriteNode>> children;
};

struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::Shared<xx::GLTexture> t;
	SpriteNode n;
	xx::LineStrip ls;
};
