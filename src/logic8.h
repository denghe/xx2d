#pragma once
#include "pch.h"
#include "logic_base.h"

struct Logic8;
struct Mices;

struct Mice {
	xx::AffineTransform at{};
	xx::XY pos{}, scale{ 1, 1 };
	float radians{};
	xx::Label name;
	xx::Sprite body;
	void Init(Mices* owner, xx::XY const& pos, std::string&& name);
};

struct Mices {
	Logic8* logic8{};
	xx::XY pos{}, scale{ 1, 1 };
	float radians{};
	std::vector<Mice> ms;
	void Init(Logic8* logic);
	void Draw();
};

struct Logic8 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	Mices mices;
	xx::Shared<xx::GLTexture> t;
};
