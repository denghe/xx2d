#include "xx2d.h"

namespace xx {

	GLTexture Engine::LoadTexture(std::string_view const& fn) {
		auto [d, p] = ReadAllBytes(fn);
		return LoadGLTexture(d, p);
	}

	xx::Shared<GLTexture> Engine::LoadSharedTexture(std::string_view const& fn) {
		return xx::Make<GLTexture>(LoadTexture(fn));
	}

	xx::Shared<GLTexture> Engine::LoadTextureFromCache(std::string_view const& fn) {
		auto p = GetFullPath(fn);
		if (p.empty()) throw std::logic_error("fn can't find: " + std::string(fn));
		if (auto iter = textureCache.find(p); iter != textureCache.end())
			return iter->second;
		else {
			auto t = xx::Make<GLTexture>(LoadGLTexture(ReadAllBytesWithFullPath(p), p));
			textureCache.emplace(std::move(p), t);
			return t;
		}
	}


	void Engine::UnloadTextureFromCache(std::string_view const& fn) {
		if (auto p = GetFullPath(fn); !p.empty()) {
			textureCache.erase(p);
		}
	}


	void Engine::UnloadTextureFromCache(xx::Shared<GLTexture> const& t) {
		textureCache.erase(std::get<std::string>(t->vs));
	}


	size_t Engine::RemoveUnusedFromTextureCache() {
		size_t counter{};
		for (auto&& iter = textureCache.begin(); iter != textureCache.end();) {
			if (iter->second.GetSharedCount() == 1) {
				iter = textureCache.erase(iter);
				++counter;
			} else {
				++iter;
			}
		}
		return counter;
	}

}
