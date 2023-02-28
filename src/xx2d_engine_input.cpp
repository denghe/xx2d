#include "xx2d_pch.h"

namespace xx {

	bool Engine::Pressed(Mbtns const& b) {
		return mbtnStates[(size_t)b];
	}

	bool Engine::Pressed(KbdKeys const& k) {
		return kbdStates[(size_t)k];
	}

}
