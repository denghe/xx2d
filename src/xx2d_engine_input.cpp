#include "xx2d.h"

namespace xx {

	bool Engine::Pressed(Mbtns const& b) {
		return mbtnStates[(size_t)b];
	}

	bool Engine::Pressed(KbdKeys const& k) {
		return kbdStates[(size_t)k];
	}

}
