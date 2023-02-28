#include "xx2d_pch.h"

namespace xx {

	BMFont Engine::LoadBMFont(std::string_view const& fn) {
		BMFont o;
		o.Load(fn);
		return o;
	}

}
