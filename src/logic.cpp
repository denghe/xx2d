#include "pch.h"
#include "logic.h"
#include "tmxdata.h"

void Logic::Init() {
	coros.Add([](Logic* self)->CoType {
		CoYield;
	}(this));
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;
	coros();
	return 0;
}
