#include "pch.h"

TPData Engine::LoadTPData(std::string_view const& fn) {
	auto [d, p] = ReadAllBytes(fn);
	TPData c;
	std::string rootPath;
	if (auto i = p.find_last_of("/"); i != p.npos) {
		rootPath = p.substr(0, i + 1);
	}
	if (auto r = c.Fill(d, rootPath)) throw std::logic_error(xx::ToString("texture packer's plist file fill error. r = ", r, ". file = ", p));
	return c;
}


void Engine::LoadFramesFromCache(TPData const& tpd) {
	auto t = xx::Make<GLTexture>(LoadTexture(tpd.realTextureFileName));
	for (auto& o : tpd.frames) {
		auto&& result = frameCache.emplace(o->key, o);
		if (!result.second) throw std::logic_error(xx::ToString("duplicated key in frameCache. key = ", o->key, ". plist fn = ", std::get<std::string>(t->vs)));
		o->tex = t;
	}
}


void Engine::UnloadFramesFromCache(TPData const& tpd) {
	for (auto const& o : tpd.frames) {
		frameCache.erase(o->key);
	}
}


size_t Engine::RemoveUnusedFromFrameCache() {
	size_t counter = 0;
	for (auto&& iter = frameCache.begin(); iter != frameCache.end();) {
		if (iter->second.GetSharedCount() == 1) {
			iter = frameCache.erase(iter);
			++counter;
		} else {
			++iter;
		}
	}
	return counter;
}
