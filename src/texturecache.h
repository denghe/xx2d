#pragma once
#include "pch.h"

struct TextureCache : std::unordered_map<std::string, xx::Shared<GLTexture>, xx::StringHasher<>, std::equal_to<void>> {
};
