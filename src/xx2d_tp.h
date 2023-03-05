#pragma once
#include "xx2d_pch.h"

namespace xx {

	// texture packer's data container. fill data from export for cocos 3.x .plist file
	struct TP {
		std::vector<xx::Shared<Frame>> frames;
		bool premultiplyAlpha;
		std::string realTextureFileName;

		void Fill(std::string_view plistFn, bool sortByName = true);

		int Fill(std::string_view text, std::string_view const& rootPath);

		// format: a,b
		template<typename T>
		static int ReadFloat2(std::string_view const& line, T& a, T& b) {
			auto dotPos = line.find(',', 1);
			if (dotPos == std::string_view::npos) return __LINE__;
#ifdef _MSVC
			std::from_chars(line.data(), line.data() + dotPos, a);
			std::from_chars(line.data() + dotPos + 1, line.data() + line.size(), b);
#else
            a = atof(std::string(line.substr(0, dotPos)).c_str());
            b = atof(std::string(line.substr(dotPos + 1)).c_str());
#endif
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
			vs.push_back((T)ReadInteger<true>(p));
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

}
