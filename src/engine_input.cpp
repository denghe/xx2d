#pragma once
#include "pch.h"

bool Engine::Pressed(Mbtns const& b) {
	return mbtnStatus[(size_t)b];
}

bool Engine::Pressed(KbdKeys const& k) {
	return kbdStates[(size_t)k];
}
