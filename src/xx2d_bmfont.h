﻿#pragma once
#include "xx2d.h"

namespace xx {

	struct Engine;
	struct BMFont {
		struct Char {
			// uint32_t id;
			uint16_t x, y, width, height;
			int16_t xoffset, yoffset, xadvance;
			uint8_t page, chnl;
		};
		std::array<Char, 256> charArray;	// charMap ascii cache
		std::unordered_map<uint32_t, Char> charMap;	// key: char id
		std::unordered_map<uint64_t, int> kernings;	// key: char id pair
		std::vector<xx::Shared<GLTexture>> texs;
		uint8_t paddingLeft{}, paddingTop{}, paddingRight{}, paddingBottom{};
		int16_t fontSize{};
		uint16_t lineHeight{};

		// load binary .fnt & texture from .fnt file
		void Load(std::string_view const& fn);

		// texture index: page
		Char const* GetChar(char32_t const& charId) const;
	};

}
