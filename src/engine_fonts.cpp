#include "pch.h"

BMFont Engine::LoadBMFont(std::string_view const& fn) {
	BMFont o;
	o.Load(this, fn);
	return o;
}
