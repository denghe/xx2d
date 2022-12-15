#pragma once
#include "pch.h"

// texture packer's data container. can fill data from .plist file. support export for cocos 3.x file, DO NOT REMOVE ANY SPACE ( REFORMAT ) !!!
struct TPData {

	struct Item {
		std::string name;
		// std::vector<std::string> aliases;
		XY spriteOffset;
		Size spriteSize;
		Size spriteSourceSize;
		Rect textureRect;
		bool textureRotated;
		std::vector<uint16_t> triangles;
		std::vector<float> vertices;
		std::vector<float> verticesUV;
	};

	enum class PixelFormats : uint8_t {
		RGBA8888	// more?
	};

	int format;
	PixelFormats pixelFormat;
	bool premultiplyAlpha;
	std::string realTextureFileName;
	Size size;
	//std::string smartupdate;
	std::string textureFileName;
	std::vector<Item> items;

	static bool NextLine(std::string_view const& text, size_t& offset, std::string_view& out) {
	LabBegin:
		auto pos = text.find('\n', offset);
		if (pos == std::string_view::npos) {    // eof
			out = std::string_view(text.data() + offset, text.size() - offset);
			return text.size() > offset;
		}
		if (pos == offset) {                    // skip empty line
			++offset;
			goto LabBegin;
		}
		out = std::string_view(text.data() + offset, pos - offset);
		offset = pos + 1;
		return true;
	}

	static void SkipLines(std::string_view const& text, size_t& offset, size_t n) {
		while (n) {
			auto pos = text.find('\n', offset);
			if (pos == std::string_view::npos) return;
			offset = pos + 1;
			--n;
		}
	}

	// format: a,b
	template<typename T>
	static int ReadFloat2(std::string_view const& line, T& a, T& b) {
		auto dotPos = line.find(',', 1);
		if (dotPos == std::string_view::npos) return __LINE__;
		std::from_chars(line.data(), line.data() + dotPos, a);
		std::from_chars(line.data() + dotPos + 1, line.data() + line.size(), b);
		return 0;
	}

	template<bool check9 = false>
	static int ReadInteger(const char*& p) {
		int x = 0;
		while (*p >= '0') {
			if constexpr (check9) {
				if (*p > '9') break;
			}
			x = (x * 10) + (*p - '0');
			++p;
		}
		return x;
	}

	// format: a,b
	// ascii£º ',' < '0~9' < '>}'
	template<typename T>
	static int ReadInteger2(std::string_view const& line, T& a, T& b) {
		auto&& p = line.data();
		a = (T)ReadInteger(p);
		++p;	// skip ','
		b = (T)ReadInteger<true>(p);
		return 0;
	}

	// format: a,b},{c,d
	template<typename T>
	static void ReadInteger4(std::string_view const& line, T& a, T& b, T& c, T& d) {
		auto&& p = line.data();
		a = (T)ReadInteger(p);
		++p;	// skip ','
		b = (T)ReadInteger<true>(p);
		p += 3;	// skip '},{'
		c = (T)ReadInteger(p);
		++p;	// skip ','
		d = (T)ReadInteger<true>(p);
	}

	static int SubStr(std::string_view& line, size_t const& from, size_t const& to) {
		if (line.size() <= (from + to)) return __LINE__;
		line = std::string_view(line.data() + from, (line.size() - (from + to)));
		return 0;
	}

	static bool ReadBool(std::string_view& line, size_t const& numSpaces) {
		return line[numSpaces + 1] == 't';
	}

	// ' ' continue£¬'<' quit. ascii£º ' ' < '0-9' < '<'
	template<typename T>
	static void SpaceSplitFillVector(std::vector<T>& vs, std::string_view const& line) {
		vs.clear();
		auto&& p = line.data();
	LabBegin:
		vs.push_back((T)ReadInteger(p));
		if (*p == ' ') {
			++p;
			goto LabBegin;
		}
	}

	int Fill(std::string_view const& text, std::string_view const& texPreFixPath) {
		items.clear();
		size_t offset = 0, siz = text.size();
		std::string_view line;
		while (NextLine(text, offset, line)) {
			if (line == "    <dict>") {                                         // locate to data started
				SkipLines(text, offset, 2);                                     // skip <key>frames</key> & <dict>

			LabBegin:
				auto&& o = items.emplace_back();
				if (!NextLine(text, offset, line)) return __LINE__;             // locate to <key>???????</key>
				if (SubStr(line, 17, 6)) return __LINE__;                       // cut item.name
				o.name = line;

				SkipLines(text, offset, 4);                                     // skip <dict> & <key>aliases</key> & <array/> & <key>spriteOffset</key>
				if (!NextLine(text, offset, line)) return __LINE__;             // <string>{x,y}</string>
				if (SubStr(line, 25, 10)) return __LINE__;                      // cut x,y
				if (ReadFloat2(line, o.spriteOffset.x, o.spriteOffset.y)) return __LINE__;

				SkipLines(text, offset, 1);                                     // skip <key>spriteSize</key>
				if (!NextLine(text, offset, line)) return __LINE__;             // <string>{w,h}</string>
				if (SubStr(line, 25, 10)) return __LINE__;                      // cut w,h
				if (ReadInteger2(line, o.spriteSize.w, o.spriteSize.h)) return __LINE__;

				SkipLines(text, offset, 1);                                     // skip <key>spriteSourceSize</key>
				if (!NextLine(text, offset, line)) return __LINE__;             // <string>{w,h}</string>
				if (SubStr(line, 25, 10)) return __LINE__;                      // cut w,h
				if (ReadInteger2(line, o.spriteSourceSize.w, o.spriteSourceSize.h)) return __LINE__;

				SkipLines(text, offset, 1);                                     // skip <key>textureRect</key>
				if (!NextLine(text, offset, line)) return __LINE__;             // <string>{{x,y},{w,h}}</string>
				if (SubStr(line, 26, 11)) return __LINE__;                      // cut x,y},{w,h
				ReadInteger4(line.data(), o.textureRect.x, o.textureRect.y, o.textureRect.w, o.textureRect.h);

				SkipLines(text, offset, 1);                                     // skip <key>textureRotated</key>
				if (!NextLine(text, offset, line)) return __LINE__;             // <true/> or <false/>
				o.textureRotated = ReadBool(line, 16);

				if (!NextLine(text, offset, line)) return __LINE__;             // <key>triangles</key> | </dict>
				if (line == "                <key>triangles</key>") {
					if (!NextLine(text, offset, line)) return __LINE__;         // <string>27 29 7 27 1...</string>
					if (SubStr(line, 24, 9)) return __LINE__;                   // cut numbers
					SpaceSplitFillVector(o.triangles, line);

					SkipLines(text, offset, 1);                                 // skip <key>vertices</key>
					if (!NextLine(text, offset, line)) return __LINE__;         // <string>471 408...</string>
					if (SubStr(line, 24, 9)) return __LINE__;                   // cut numbers
					SpaceSplitFillVector(o.vertices, line);

					SkipLines(text, offset, 1);                                 // skip <key>verticesUV</key>
					if (!NextLine(text, offset, line)) return __LINE__;         // <string>1280 1321...</string>
					if (SubStr(line, 24, 9)) return __LINE__;                   // cut numbers
					SpaceSplitFillVector(o.verticesUV, line);

					if (!NextLine(text, offset, line)) return __LINE__;         // </dict>
				}

				if (line == "            </dict>") {
					auto bak = offset;
					if (!NextLine(text, offset, line)) return __LINE__;         // <key> | </dict>

					if (line == "        </dict>") {                            // items fill end, begin fill meta
						SkipLines(text, offset, 3);                             // skip <key>metadata</key>    <dict>     <key>format</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <integer>???</integer>
						if (SubStr(line, 21, 10)) return __LINE__;              // cut number
						auto&& p = line.data();
						format = ReadInteger(p);

						SkipLines(text, offset, 1);                             // skip <key>pixelFormat</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <string>RGBA8888</string>
						if (line == "            <string>RGBA8888</string>") {
							pixelFormat = PixelFormats::RGBA8888;
						}
						else return __LINE__;

						SkipLines(text, offset, 1);                             // skip <key>premultiplyAlpha</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <true/> | <false/>
						premultiplyAlpha = ReadBool(line, 12);

						SkipLines(text, offset, 1);                             // skip <key>realTextureFileName</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <string>abc123.png</string>
						if (SubStr(line, 20, 9)) return __LINE__;               // cut fileName
						realTextureFileName = texPreFixPath;
						realTextureFileName.append(line);

						SkipLines(text, offset, 1);                             // skip <key>size</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <string>{w,h}</string>
						if (SubStr(line, 21, 10)) return __LINE__;              // cut w,h
						ReadInteger2(line, size.w, size.h);

						SkipLines(text, offset, 3);                             // skip <key>smartupdate</key>  ...... <key>textureFileName</key>
						if (!NextLine(text, offset, line)) return __LINE__;     // <string>abc123.png</string>
						if (SubStr(line, 20, 9)) return __LINE__;               // cut fileName
						textureFileName = line;

						return 0;
					}
					else {
						offset = bak;
						goto LabBegin;
					}
				}
				else return __LINE__;
			}
		}
		return __LINE__;
	}
};
