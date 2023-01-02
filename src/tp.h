#pragma once
#include "pch.h"

// texture packer's data container. fill data from export for cocos 3.x .plist file
struct TP {
	std::vector<xx::Shared<Frame>> frames;
	bool premultiplyAlpha;
	std::string realTextureFileName;

	int Fill(std::string_view text, std::string_view const& texPreFixPath) {
		frames.clear();
		size_t i, j;

		if (i = text.find(">frames<"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 19);													// skip    >frames</key><dict>

		if (i = text.find("<key>"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

	LabBegin:
		auto&& o = *frames.emplace_back().Emplace();
		if (j = text.find('<'); j == text.npos) return __LINE__;				// </key>
		o.key = CutStr(text, 0, j);
		CutStr(text, j + 30);													// skip    </key><dict><key>aliases</key>

		if (i = text.find("/>"sv); i == text.npos) return __LINE__;				// <array/>
		CutStr(text, i + 2);													// skip    <array/>

		if (i = text.find('<'); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

		if (text.starts_with('a')) {											// anchor
			CutStr(text, 20);													// skip    anchor</key><string>

			if (i = text.find('{'); i == text.npos) return __LINE__;
			if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
			o.anchor.emplace();
			if (ReadFloat2(CutStr(text, i + 1, j - i - 1), o.anchor->x, o.anchor->y)) return __LINE__;
			CutStr(text, j + 10);												// skip    }</string>
		}

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadFloat2(CutStr(text, i + 1, j - i - 1), o.spriteOffset.x, o.spriteOffset.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSize.w, o.spriteSize.h)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSourceSize.w, o.spriteSourceSize.h)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find("{{"sv); i == text.npos) return __LINE__;
		if (j = text.find("}}"sv, i + 10); j == text.npos) return __LINE__;
		ReadInteger4(CutStr(text, i + 2, j - i - 2), o.textureRect.x, o.textureRect.y, o.textureRect.w, o.textureRect.h);
		CutStr(text, j + 11);													// skip    }}</string>

		if (i = text.find("</k"sv); i == text.npos) return __LINE__;			// <key>textureRotated</key>
		CutStr(text, i + 6);													// skip    </key>

		if (i = text.find('<'); i == text.npos) return __LINE__;				// locate <true || <false
		CutStr(text, i + 1);													// skip    <

		if (text.starts_with('t')) {
			o.textureRotated = true;
			CutStr(text, 6);													// skip    true/>
		} else {
			o.textureRotated = false;
			CutStr(text, 7);													// skip    false/>
		}

		if (i = text.find("<k"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

		if (text.starts_with('t')) {
			CutStr(text, 15);													// skip    triangles</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.triangles, CutStr(text, i + 8, j - i - 8));
			CutStr(text, j + 28);												// skip    </string><key>vertices</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.vertices, CutStr(text, i + 8, j - i - 8));
			CutStr(text, j + 30);												// skip    </string><key>verticesUV</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.verticesUV, CutStr(text, i + 8, j - i - 8));
			CutStr(text, j + 9);												// skip    </string>
		}

		if (!text.starts_with("metadata<"sv)) {									// <key>metadata</key>    ||    <key> frame name </key>
			goto LabBegin;
		}

		CutStr(text, 20);														// skip    metadata</key><dict>

		if (i = text.find(">pre"sv); i == text.npos) return __LINE__;			// <key>premultiplyAlpha</key>
		CutStr(text, i + 23);													// skip    >premultiplyAlpha</key>

		if (i = text.find('<'); i == text.npos) return __LINE__;				// locate <true || <false
		CutStr(text, i + 1);													// skip    <

		if (text.starts_with('t')) {
			premultiplyAlpha = true;
			CutStr(text, 6);													// skip    true/>
		} else {
			premultiplyAlpha = false;
			CutStr(text, 7);													// skip    false/>
		}

		// realTextureFileName
		if (i = text.find("<s"sv); i == text.npos) return __LINE__;				// <string> tex file name .ext
		if (j = text.find('<', i + 8); j == text.npos) return __LINE__;			// </string>
		realTextureFileName = texPreFixPath;
		realTextureFileName.append(CutStr(text, i + 8, j - i - 8));

		return 0;
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
	// ascii： ',' < '0~9' < '>}'
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

	// ' ' continue，'<' quit. ascii： ' ' < '0-9' < '<'
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

	// unsafe for speed up
	XX_INLINE static void CutStr(std::string_view& sv, size_t const& i) {
		sv = { sv.data() + i, sv.size() - i };
	}
	XX_INLINE static std::string_view CutStr(std::string_view const& sv, size_t const& i, size_t const& siz) {
		return { sv.data() + i, siz };
	}
};
