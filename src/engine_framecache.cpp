#include "pch.h"

TPData Engine::LoadTPData(std::string_view const& fn) {
	auto [d, p] = ReadAllBytes(fn);
	TPData c;
	if (auto r = c.Fill(d)) throw std::logic_error(xx::ToString("texture packer's plist file fill error. r = ", r));
	return c;
}


void Engine::LoadFramesFromCache(TPData const& tpd) {


	//std::string p;
	//auto t = xx::Make<GLTexture>(LoadTexture(tpd.realTextureFileName, &p));
	//for (auto& o : tpd.items) {

	//}
}


void Engine::UnloadFramesFromCache(TPData const& tpd) {

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
