#include "xx_typehelpers.h"
#include <charconv>
//#include "fast_double_parser.h"

// texture packer's special plist reader

namespace TexturePacker {
	struct Size { float w, h; };
	struct Vec2 { float x, y; };
	struct Vec3 { float x, y, z; };
	struct Rect { Vec2 pos;	Size siz; };

	struct Item {
		std::string name;
		// std::vector<std::string> aliases;
		Vec2 spriteOffset;
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

	struct Context {
		int format;
		PixelFormats pixelFormat;
		bool premultiplyAlpha;
		std::string realTextureFileName;
		Size size;
		//std::string smartupdate;
		std::string textureFileName;
		std::vector<Item> items;

		XX_FORCEINLINE static bool NextLine(std::string_view const& text, size_t& offset, std::string_view& out) {
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

		XX_FORCEINLINE static void SkipLines(std::string_view const& text, size_t& offset, size_t n) {
			while (n) {
				auto pos = text.find('\n', offset);
				if (pos == std::string_view::npos) return;
				offset = pos + 1;
				--n;
			}
		}

		template<bool check9 = false>
		XX_FORCEINLINE static int ReadInteger(const char*& p) {
			int x = 0;
			while (*p >= '0' && check9 && *p <= '9') {
				x = (x * 10) + (*p - '0');
				++p;
			}
			return x;
		}

		// 格式为 a,b
		template<typename T>
		XX_FORCEINLINE static int ReadFloat2(std::string_view const& line, T& a, T& b) {
			//double d;
			//if (auto p = fast_double_parser::parse_number(line.data(), &d)) {
			//	a = (T)d;
			//	if (p = fast_double_parser::parse_number(p + 1, &d)) {
			//		b = (T)d;
			//		return 0;
			//	}
			//}
			//return __LINE__;
			auto dotPos = line.find(',', 1);
			if (dotPos == std::string_view::npos) return __LINE__;
			std::from_chars(line.data(), line.data() + dotPos, a);
			std::from_chars(line.data() + dotPos + 1, line.data() + line.size(), b);
			return 0;
		}

		// 格式为 a,b
		template<typename T>
		XX_FORCEINLINE static int ReadInteger2(std::string_view const& line, T& a, T& b) {
			auto&& p = line.data();
			a = (T)ReadInteger(p);
			++p;	// skip ','
			b = (T)ReadInteger<true>(p);
			return 0;
		}

		// 格式为 a,b},{c,d
		template<typename T>
		XX_FORCEINLINE static void ReadInteger4(std::string_view const& line, T& a, T& b, T& c, T& d) {
			auto&& p = line.data();
			a = (T)ReadInteger(p);
			++p;	// skip ','
			b = (T)ReadInteger<true>(p);
			p += 3;	// skip '},{'
			c = (T)ReadInteger(p);
			++p;	// skip ','
			d = (T)ReadInteger<true>(p);
		}

		XX_FORCEINLINE static int Trim(std::string_view& line, size_t const& from, size_t const& to) {
			if (line.size() <= (from + to)) return __LINE__;
			line = std::string_view(line.data() + from, (line.size() - (from + to)));
			return 0;
		}

		XX_FORCEINLINE static bool ReadBool(std::string_view& line, size_t const& numSpaces) {
			return line[numSpaces + 1] == 't';
		}

		// 因为用到该函数的地方只可能读出整数值，直接走 int parser 逻辑. 遇到 ' ' 继续，遇到 '<' 退出. ascii 关系： ' ' < '0-9' < '<'
		template<typename T>
		XX_FORCEINLINE static void SpaceSplitFillVector(std::vector<T>& vs, std::string_view const& line) {
			vs.clear();
			auto&& p = line.data();
		LabBegin:
			vs.push_back((T)ReadInteger(p));
			if (*p == ' ') {
				++p;
				goto LabBegin;
			}
		}

		int Fill(std::string_view const& text) {
			try {
				items.clear();
				size_t offset = 0, siz = text.size();
				std::string_view line;
				while (NextLine(text, offset, line)) {
					if (line == "    <dict>") {                                         // 定位到数据起始点
						SkipLines(text, offset, 2);                                     // 跳过 <key>frames</key> 和 <dict> 这 2 行

					LabBegin:
						auto&& o = items.emplace_back();
						if (!NextLine(text, offset, line)) return __LINE__;             // 定位到 <key>???????</key>
						if (Trim(line, 17, 6)) return __LINE__;                         // 剥离出 item.name 然后保存
						o.name = line;

						SkipLines(text, offset, 4);                                     // 跳过 <dict> 和 <key>aliases</key> 和 <array/> 和 <key>spriteOffset</key> 这 4 行
						if (!NextLine(text, offset, line)) return __LINE__;             // <string>{x,y}</string>
						if (Trim(line, 25, 10)) return __LINE__;                        // 剥离出 x,y 然后保存
						if (ReadFloat2(line, o.spriteOffset.x, o.spriteOffset.y)) return __LINE__;

						SkipLines(text, offset, 1);                                     // 跳过 <key>spriteSize</key>
						if (!NextLine(text, offset, line)) return __LINE__;             // <string>{w,h}</string>
						if (Trim(line, 25, 10)) return __LINE__;                        // 剥离出 w,h 然后保存
						if (ReadInteger2(line, o.spriteSize.w, o.spriteSize.h)) return __LINE__;

						SkipLines(text, offset, 1);                                     // 跳过 <key>spriteSourceSize</key>
						if (!NextLine(text, offset, line)) return __LINE__;             // <string>{w,h}</string>
						if (Trim(line, 25, 10)) return __LINE__;                        // 剥离出 w,h 然后保存
						if (ReadInteger2(line, o.spriteSourceSize.w, o.spriteSourceSize.h)) return __LINE__;

						SkipLines(text, offset, 1);                                     // 跳过 <key>textureRect</key>
						if (!NextLine(text, offset, line)) return __LINE__;             // <string>{{x,y},{w,h}}</string>
						if (Trim(line, 26, 11)) return __LINE__;                        // 剥离出 x,y},{w,h 然后保存
						ReadInteger4(line.data(), o.textureRect.pos.x, o.textureRect.pos.y, o.textureRect.siz.w, o.textureRect.siz.h);

						SkipLines(text, offset, 1);                                     // 跳过 <key>textureRotated</key>
						if (!NextLine(text, offset, line)) return __LINE__;             // <true/> 或 <false/>
						o.textureRotated = ReadBool(line, 16);

						if (!NextLine(text, offset, line)) return __LINE__;             // <key>triangles</key> 或 </dict>
						if (line == "                <key>triangles</key>") {
							if (!NextLine(text, offset, line)) return __LINE__;         // <string>27 29 7 27 1...</string>
							if (Trim(line, 24, 9)) return __LINE__;                     // 剥离到只剩 空格间隔的数字 然后填充
							SpaceSplitFillVector(o.triangles, line);

							SkipLines(text, offset, 1);                                 // 跳过 <key>vertices</key>
							if (!NextLine(text, offset, line)) return __LINE__;         // <string>471 408...</string>
							if (Trim(line, 24, 9)) return __LINE__;                     // 剥离到只剩 空格间隔的数字 然后填充
							SpaceSplitFillVector(o.vertices, line);

							SkipLines(text, offset, 1);                                 // 跳过 <key>verticesUV</key>
							if (!NextLine(text, offset, line)) return __LINE__;         // <string>1280 1321...</string>
							if (Trim(line, 24, 9)) return __LINE__;                     // 剥离到只剩 空格间隔的数字 然后填充
							SpaceSplitFillVector(o.verticesUV, line);

							if (!NextLine(text, offset, line)) return __LINE__;         // </dict>
						}

						if (line == "            </dict>") {
							auto bak = offset;
							if (!NextLine(text, offset, line)) return __LINE__;         // <key> 或 </dict>

							if (line == "        </dict>") {                            // items 填充结束, 开始填 meta
								SkipLines(text, offset, 3);                             // 跳过 <key>metadata</key>    <dict>     <key>format</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <integer>???</integer>
								if (Trim(line, 21, 10)) return __LINE__;                // 剥离到只剩 数字 然后填充
								auto&& p = line.data();
								format = ReadInteger(p);

								SkipLines(text, offset, 1);                             // 跳过 <key>pixelFormat</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <string>RGBA8888</string>
								if (line == "            <string>RGBA8888</string>") {
									pixelFormat = PixelFormats::RGBA8888;
								}
								else return __LINE__;

								SkipLines(text, offset, 1);                             // 跳过 <key>premultiplyAlpha</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <true/> 或 <false/>
								premultiplyAlpha = ReadBool(line, 12);

								SkipLines(text, offset, 1);                             // 跳过 <key>realTextureFileName</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <string>abc123.png</string>
								if (Trim(line, 20, 9)) return __LINE__;                 // 剥离到只剩 fileName 后保存
								realTextureFileName = line;

								SkipLines(text, offset, 1);                             // 跳过 <key>size</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <string>{w,h}</string>
								if (Trim(line, 21, 10)) return __LINE__;                // 剥离到只剩 w,h 后保存
								ReadInteger2(line, size.w, size.h);

								SkipLines(text, offset, 3);                             // 跳过  <key>smartupdate</key>  ...... <key>textureFileName</key>
								if (!NextLine(text, offset, line)) return __LINE__;     // <string>abc123.png</string>
								if (Trim(line, 20, 9)) return __LINE__;                 // 剥离到只剩 fileName 后保存
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
			}
			catch (std::exception const& ex) {
				return __LINE__;
			}
			return __LINE__;
		}
	};

}

auto str1 = R"-(
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>frames</key>
        <dict>
            <key>longxiajiangjun_move (10).png</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{0.5,-7}</string>
                <key>spriteSize</key>
                <string>{486,413}</string>
                <key>spriteSourceSize</key>
                <string>{511,427}</string>
                <key>textureRect</key>
                <string>{{1,1252},{486,413}}</string>
                <key>textureRotated</key>
                <false/>
                <key>triangles</key>
                <string>27 28 29 25 26 27 27 1 4 24 25 5 20 22 23 20 21 22 23 16 17 20 17 19 16 5 15 19 17 18 20 23 17 23 24 16 16 24 5 25 27 5 13 14 15 11 12 13 9 10 11 6 9 11 6 8 9 6 7 8 6 11 13 5 6 13 5 13 15 4 5 27 2 3 4 1 2 4 27 0 1 0 27 29</string>
                <key>vertices</key>
                <string>471 42 499 98 499 140 469 200 435 203 386 224 433 250 474 250 499 310 499 362 471 408 433 427 365 427 343 412 305 413 273 360 231 340 137 336 39 320 13 292 13 172 21 86 112 85 201 106 233 106 283 80 304 37 344 38 369 14 415 14</string>
                <key>verticesUV</key>
                <string>459 1280 487 1336 487 1378 457 1438 423 1441 374 1462 421 1488 462 1488 487 1548 487 1600 459 1646 421 1665 353 1665 331 1650 293 1651 261 1598 219 1578 125 1574 27 1558 1 1530 1 1410 9 1324 100 1323 189 1344 221 1344 271 1318 292 1275 332 1276 357 1252 403 1252</string>
            </dict>
            <key>longxiajiangjun_move (147).png</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{-2,-7}</string>
                <key>spriteSize</key>
                <string>{489,413}</string>
                <key>spriteSourceSize</key>
                <string>{511,427}</string>
                <key>textureRect</key>
                <string>{{1,837},{489,413}}</string>
                <key>textureRotated</key>
                <false/>
                <key>triangles</key>
                <string>26 27 28 32 24 25 23 32 29 22 29 20 20 21 22 18 30 16 16 17 18 18 19 30 31 14 15 16 30 31 31 15 16 11 12 13 6 9 10 6 10 11 14 31 25 14 5 13 6 7 8 9 6 8 3 4 5 2 3 5 0 1 2 0 2 5 28 0 5 6 11 13 13 5 6 29 30 19 29 19 20 22 23 29 23 24 32 31 32 25 5 25 26 14 25 5 28 5 26</string>
                <key>vertices</key>
                <string>414 15 475 44 498 95 498 141 469 200 375 220 418 247 469 246 498 311 498 361 456 427 340 427 300 408 288 370 248 349 186 345 156 329 31 336 11 291 12 191 29 153 9 129 22 102 68 87 142 87 237 105 284 82 301 44 364 14 72 149 99 175 132 174 139 145</string>
                <key>verticesUV</key>
                <string>406 838 467 867 490 918 490 964 461 1023 367 1043 410 1070 461 1069 490 1134 490 1184 448 1250 332 1250 292 1231 280 1193 240 1172 178 1168 148 1152 23 1159 3 1114 4 1014 21 976 1 952 14 925 60 910 134 910 229 928 276 905 293 867 356 837 64 972 91 998 124 997 131 968</string>
            </dict>
            <key>longxiajiangjun_move (24).png</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{-0.5,-5}</string>
                <key>spriteSize</key>
                <string>{488,417}</string>
                <key>spriteSourceSize</key>
                <string>{511,427}</string>
                <key>textureRect</key>
                <string>{{1,1},{488,417}}</string>
                <key>textureRotated</key>
                <false/>
                <key>triangles</key>
                <string>4 22 23 20 21 22 16 18 19 19 15 16 4 20 22 16 17 18 19 14 15 19 5 14 14 5 13 19 4 5 10 6 8 6 10 11 10 8 9 6 7 8 11 13 5 11 5 6 13 11 12 19 20 4 2 3 4 1 2 4 1 4 23 0 1 23</string>
                <key>vertices</key>
                <string>474 39 499 90 499 140 473 198 379 213 376 227 434 244 473 246 499 310 499 372 444 427 343 427 299 405 280 364 232 341 174 352 88 352 11 325 12 107 237 98 281 81 304 40 369 10 415 10</string>
                <key>verticesUV</key>
                <string>464 30 489 81 489 131 463 189 369 204 366 218 424 235 463 237 489 301 489 363 434 418 333 418 289 396 270 355 222 332 164 343 78 343 1 316 2 98 227 89 271 72 294 31 359 1 405 1</string>
            </dict>
            <key>longxiajiangjun_move (72).png</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{-2,-6}</string>
                <key>spriteSize</key>
                <string>{489,415}</string>
                <key>spriteSourceSize</key>
                <string>{511,427}</string>
                <key>textureRect</key>
                <string>{{1,420},{489,415}}</string>
                <key>textureRotated</key>
                <false/>
                <key>triangles</key>
                <string>28 29 30 34 26 27 25 34 31 24 31 22 22 23 24 20 32 18 18 19 20 20 21 32 33 16 17 18 32 33 33 17 18 13 14 15 11 12 13 6 9 10 6 10 11 6 11 13 16 33 27 16 5 15 6 7 8 9 6 8 3 4 5 2 3 5 0 1 2 0 2 5 30 0 5 6 13 15 15 5 6 31 32 21 31 21 22 24 25 31 25 26 34 33 34 27 5 27 28 16 27 5 30 5 28</string>
                <key>vertices</key>
                <string>414 13 475 42 498 93 498 139 469 198 375 218 418 245 469 244 498 309 498 359 465 410 430 427 364 427 338 411 302 412 287 367 248 347 186 343 156 327 31 334 11 289 12 189 29 151 9 127 22 100 68 85 142 85 237 103 284 80 301 42 364 12 72 147 99 173 132 172 139 143</string>
                <key>verticesUV</key>
                <string>406 421 467 450 490 501 490 547 461 606 367 626 410 653 461 652 490 717 490 767 457 818 422 835 356 835 330 819 294 820 279 775 240 755 178 751 148 735 23 742 3 697 4 597 21 559 1 535 14 508 60 493 134 493 229 511 276 488 293 450 356 420 64 555 91 581 124 580 131 551</string>
            </dict>
            <key>longxiajiangjun_move (85).png</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{0.5,-7}</string>
                <key>spriteSize</key>
                <string>{486,413}</string>
                <key>spriteSourceSize</key>
                <string>{511,427}</string>
                <key>textureRect</key>
                <string>{{1,1667},{486,413}}</string>
                <key>textureRotated</key>
                <false/>
                <key>triangles</key>
                <string>27 28 29 25 26 27 27 1 4 24 25 5 20 22 23 20 21 22 23 16 17 20 17 19 16 5 15 19 17 18 20 23 17 23 24 16 16 24 5 25 27 5 13 14 15 11 12 13 9 10 11 6 9 11 6 8 9 6 7 8 6 11 13 5 6 13 5 13 15 4 5 27 2 3 4 1 2 4 27 0 1 0 27 29</string>
                <key>vertices</key>
                <string>471 42 499 98 499 140 469 200 435 203 386 224 433 250 474 250 499 310 499 362 471 408 433 427 365 427 343 412 305 413 273 360 231 340 137 336 39 320 13 292 13 172 21 86 112 85 201 106 233 106 283 80 304 37 344 38 369 14 415 14</string>
                <key>verticesUV</key>
                <string>459 1695 487 1751 487 1793 457 1853 423 1856 374 1877 421 1903 462 1903 487 1963 487 2015 459 2061 421 2080 353 2080 331 2065 293 2066 261 2013 219 1993 125 1989 27 1973 1 1945 1 1825 9 1739 100 1738 189 1759 221 1759 271 1733 292 1690 332 1691 357 1667 403 1667</string>
            </dict>
        </dict>
        <key>metadata</key>
        <dict>
            <key>format</key>
            <integer>3</integer>
            <key>pixelFormat</key>
            <string>RGBA8888</string>
            <key>premultiplyAlpha</key>
            <false/>
            <key>realTextureFileName</key>
            <string>longixajiangjun_move2.pkm</string>
            <key>size</key>
            <string>{491,2081}</string>
            <key>smartupdate</key>
            <string>$TexturePacker:SmartUpdate:3114bc1d4f7401da4c6bb740391a74cc:3b6408debbba34f1c7e4906fb8a0255d:c10ec4b279eacb94e7354a6bc8d85237$</string>
            <key>textureFileName</key>
            <string>longixajiangjun_move2.pkm</string>
        </dict>
    </dict>
</plist>
)-";

auto str2 = R"-(
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>frames</key>
        <dict>
            <key>group__TextView</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{0,0}</string>
                <key>spriteSize</key>
                <string>{387,88}</string>
                <key>spriteSourceSize</key>
                <string>{387,88}</string>
                <key>textureRect</key>
                <string>{{1,477},{387,88}}</string>
                <key>textureRotated</key>
                <false/>
            </dict>
            <key>prefs-image-window</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{0,0}</string>
                <key>spriteSize</key>
                <string>{434,474}</string>
                <key>spriteSourceSize</key>
                <string>{434,474}</string>
                <key>textureRect</key>
                <string>{{1,1},{434,474}}</string>
                <key>textureRotated</key>
                <false/>
            </dict>
        </dict>
        <key>metadata</key>
        <dict>
            <key>format</key>
            <integer>3</integer>
            <key>pixelFormat</key>
            <string>RGBA8888</string>
            <key>premultiplyAlpha</key>
            <false/>
            <key>realTextureFileName</key>
            <string>a.png</string>
            <key>size</key>
            <string>{436,566}</string>
            <key>smartupdate</key>
            <string>$TexturePacker:SmartUpdate:e4d958065993bc92081d1d61d2bef24e:8a1897c9dede6e7f874f22d4264ee045:3fe7819918ccd8b069678b9f17b4fc07$</string>
            <key>textureFileName</key>
            <string>a.png</string>
        </dict>
    </dict>
</plist>
)-";

int main() {
	TexturePacker::Context tpc;
	auto secs = xx::NowEpochSeconds();
	for (size_t i = 0; i < 100000; i++) {
		auto r = tpc.Fill(str1);
		//auto r = tpc.Fill(str2);
	}
	std::cout << xx::NowEpochSeconds(secs) << std::endl;
	return 0;
}



//
//template<typename T>
//struct Register {
//	Register(std::string_view const& typeName) {
//		std::cout << typeName << std::endl;
//	}
//
//	template<typename F>
//	Register& Field(std::string_view const& fieldName, F&& fieldPtr) {
//		std::cout << fieldName << std::endl;
//		return *this;
//	}
//};
//
//#define REGISTER_PREFIX_(t) Register<t>(XX_STRINGIFY(t))
//#define REGISTER_SUFFIX_(t, a) .Field(XX_STRINGIFY(a), &t::a)
//#define REGISTER_CLASS_FIELDS(...) XX_CONCAT(XX_, XX_BUGFIX(XX_NARG(__VA_ARGS__))) \
//(REGISTER_PREFIX_, REGISTER_SUFFIX_, __VA_ARGS__)
//
//struct Foo {
//	int a, b, c, d, e, f, g, h, i, j;
//};
//
//int main() {
//	REGISTER_CLASS_FIELDS(Foo, a, b, c, d, e, f, g, h, i, j);
//	Register<Foo>("Foo")
//		.Field("a", &Foo::a)
//		.Field("b", &Foo::b)
//		.Field("c", &Foo::b)
//		.Field("d", &Foo::b)
//		.Field("e", &Foo::b)
//		.Field("f", &Foo::b)
//		.Field("h", &Foo::h)
//		.Field("i", &Foo::i)
//		.Field("j", &Foo::j);
//
//	return 0;
//}








//// Create a string out of the first argument, and the rest of the arguments.
//#define TWO_STRINGS( first, ... ) #first, #__VA_ARGS__
//#define A( ... ) TWO_STRINGS(__VA_ARGS__)
//const char* c[2] = { A(1, 2) };





//template<typename T>
//struct TypeMembersName;
//
//template<typename T, size_t I>
//static constexpr std::string_view GetMemberName() {
//	size_t a = 0, b = 0, i = 0;
//	auto&& mns = TypeMembersName<T>::memberNames;
//	for (; i < mns.size(); ++i) {
//		if (mns[i] == ',') {
//			if (a < I) {
//				++a;
//				b = i + 1;
//				if (mns[b] == ' ') ++b;
//			}
//			else break;
//		}
//	}
//	return std::string_view(&mns[b], i - b);
//}
//
//template<typename T, typename...FS>
//void RegisterFields(T* self, FS&...fs) {
//	// todo: fs 依次取地址 并减去 self 从而得到 offset ?
//}
//
//
//// 下面这些代码放到宏
//template<>
//struct TypeMembersName<Foo> {
//	static constexpr std::string_view memberNames = "a, b, c";	// STRINGIFY_LIST(__VA_ARGS__)
//	//inline static std::map<std::string_view, size_t> memberOffsets;
//	struct Register_Foo : Foo {
//		Register_Foo() {
//			RegisterFields<Foo>(this, a, b, c);	// __VA_ARGS__
//		}
//	};
//	inline static Register_Foo instance_Register_Foo;
//};
