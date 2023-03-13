#include "xx2d.h"

namespace xx {

	BMFont Engine::LoadBMFont(std::string_view const& fn) {
		BMFont o;
		o.Load(fn);
		return o;
	}

}
