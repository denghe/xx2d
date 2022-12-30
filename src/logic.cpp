#include "pch.h"
#include "logic.h"
#include "tmxdata.h"

void Logic::Init() {
	coros.Add([](Logic* eg)->CoType {
		CoYield;
		TMXData t;
		t.Fill(eg, "res/tiledmap1/m1.tmx");
		std::cout << t.map.version << std::endl;
	}(this));
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;
	coros();
	return 0;
}
