#pragma once

// texture packer's special plist reader

#include <string>
#include <string_view>
#include <vector>
#include <exception>

#include <charconv>
//#include "fast_double_parser.h"

namespace TexturePackerReader {

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

		template<bool check9 = false>
		static int ReadInteger(const char*& p) {
			int x = 0;
			while (*p >= '0' && check9 && *p <= '9') {
				x = (x * 10) + (*p - '0');
				++p;
			}
			return x;
		}

		// 格式为 a,b
		template<typename T>
		static int ReadFloat2(std::string_view const& line, T& a, T& b) {
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
		static int ReadInteger2(std::string_view const& line, T& a, T& b) {
			auto&& p = line.data();
			a = (T)ReadInteger(p);
			++p;	// skip ','
			b = (T)ReadInteger<true>(p);
			return 0;
		}

		// 格式为 a,b},{c,d
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

		static int Trim(std::string_view& line, size_t const& from, size_t const& to) {
			if (line.size() <= (from + to)) return __LINE__;
			line = std::string_view(line.data() + from, (line.size() - (from + to)));
			return 0;
		}

		static bool ReadBool(std::string_view& line, size_t const& numSpaces) {
			return line[numSpaces + 1] == 't';
		}

		// 因为用到该函数的地方只可能读出整数值，直接走 int parser 逻辑. 遇到 ' ' 继续，遇到 '<' 退出. ascii 关系： ' ' < '0-9' < '<'
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
