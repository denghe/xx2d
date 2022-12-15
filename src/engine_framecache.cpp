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
	for (auto const& o : tpd.items) {
		auto&& result = frameCache.emplace(o.name, xx::Shared<Frame>{});
		if (!result.second) throw std::logic_error(xx::ToString("duplicated key in frameCache. key = ", o.name, ". plist fn = ", std::get<std::string>(t->vs)));
		auto&& f = *result.first->second.Emplace();
		f.tex = t;
		f.spriteOffset = o.spriteOffset;
		f.spriteSize = o.spriteSize;
		f.spriteSourceSize = o.spriteSourceSize;
		f.textureRect = o.textureRect;
		f.textureRotated = o.textureRotated;
	}
}


void Engine::UnloadFramesFromCache(TPData const& tpd) {
	for (auto const& o : tpd.items) {
		frameCache.erase(o.name);
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
