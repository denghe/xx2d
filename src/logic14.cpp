#include "pch.h"
#include "logic.h"
#include "logic14.h"


void Scissor::Begin() {
	xx::engine.sm.End();
	glEnable(GL_SCISSOR_TEST);
	glScissor(xx::engine.hw + rect.x, xx::engine.hh + rect.y, rect.wh.x, rect.wh.y);
}
void Scissor::End() {
	xx::engine.sm.End();
	glDisable(GL_SCISSOR_TEST);
}

void Logic14::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic14 Init( scissor tests )" << std::endl;

	spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"))
		.SetScale(3);

	scissor.rect = { -100, -100, 200, 200 };
}

int Logic14::Update() {
	scissor.Begin();
	spr.Draw();
	scissor.End();
	return 0;
}
