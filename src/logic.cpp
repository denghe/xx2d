#include "pch.h"
#include "logic.h"
#include "tmxdata.h"

void Logic::Init() {
	coros.Add([](Logic* self)->CoType {
		CoYield;
		TMXData t;
		auto&& [d, fp] = self->ReadAllBytes("res/tiledmap1/m1.tmx");
		if (!d) {
			throw std::logic_error("read file error.");
		}
		if (int r = t.Fill(d)) {
			throw std::logic_error(xx::ToString("tmxd.Fill error. r = ", r, ", fn = ", fp));
		}
		std::cout << t.map.version << std::endl;
	}(this));
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;
	coros();
	return 0;
}
