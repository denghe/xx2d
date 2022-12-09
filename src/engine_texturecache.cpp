#include "pch.h"
#include "engine.h"


xx::Shared<GLTexture> Engine::TextureCacheLoad(std::string_view fn) {
	auto p = GetFullPath(fn);
	if (p.empty()) throw std::logic_error("fn can't find: " + std::string(fn));
	if (auto iter = textureCache.find(p); iter != textureCache.end())
		return iter->second;
	else {
		auto t = xx::Make<GLTexture>(LoadTexture(p));
		textureCache.emplace(std::move(p), t);
		return t;
	}
}


void Engine::TextureCacheUnload(std::string_view fn) {
	if (auto p = GetFullPath(fn); !p.empty()) {
		textureCache.erase(p);
	}
}


void Engine::TextureCacheUnload(xx::Shared<GLTexture> const& t) {
	textureCache.erase(std::get<std::string>(t->vs));
}
