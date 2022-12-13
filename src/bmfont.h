#pragma once
#include "pch.h"

struct BMFont {
	struct Char {
		// uint32_t id;
		uint16_t x, y, width, height;
		int16_t xoffset, yoffset, xadvance;
		uint8_t page, chnl;
	};
	std::array<Char, 256> charRectArray{};	// index: ascII
	std::unordered_map<uint32_t, Char> charRectMap;	// key: unicode value
	std::unordered_map<uint64_t, int> kerningPairs;	// maybe unused ???
	std::vector<xx::Shared<GLTexture>> texs;
	uint8_t paddingLeft{}, paddingTop{}, paddingRight{}, paddingBottom{};
	int16_t fontSize{};
	uint16_t lineHeight{};

	void LoadFromFile(Engine* eg, std::string_view fn);
};
