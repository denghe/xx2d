#include "pch.h"

namespace xx {

	BMFont Engine::LoadBMFont(std::string_view const& fn) {
		BMFont o;
		o.Load(this, fn);
		return o;
	}

}
