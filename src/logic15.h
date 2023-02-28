#pragma once
#include "pch.h"
#include "logic_base.h"

struct RenderTexture {
	xx::Shared<xx::GLTexture> tex;
	void Init(xx::Pos<> wh);
	void Begin();
	void End();
};

struct Logic15 : LogicBase {
	RenderTexture rt;
	xx::Sprite spr, spr2;
	void Init(Logic* logic) override;
	int Update() override;
};
