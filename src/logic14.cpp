#include "pch.h"
#include "logic.h"
#include "logic14.h"

void Logic14::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic14 Init( png tex )" << std::endl;

	spr.SetTexture(xx::engine.LoadSharedTexture("res/tiledmap2/tree.png"));
}

int Logic14::Update() {
	spr.Draw();
	return 0;
}
