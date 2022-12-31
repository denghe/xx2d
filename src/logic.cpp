#include "pch.h"
#include "logic.h"
#include "tmxdata.h"

void Logic::Init() {
	TMXData t;
	t.Fill(this, "res/tiledmap1/m1.tmx");
	std::cout << t.map.version << std::endl;
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;
	return 0;
}
