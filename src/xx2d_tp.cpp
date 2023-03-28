#include "xx2d.h"

namespace xx {

	void TP::Fill(std::string_view plistFn, bool sortByName) {
		std::string rootPath;
		if (auto&& [d, fp] = engine.LoadFileData(plistFn); !d) {
			throw std::logic_error("read file error: " + std::string(plistFn));
		} else {
			plistFullPath = fp;
			if (auto&& i = fp.find_last_of("/"); i != fp.npos) {
				rootPath = fp.substr(0, i + 1);
			}
			if (int r = Fill(d, rootPath)) {
				throw std::logic_error(xx::ToString("parse plist file content error: r = ", r, ", fn = ", fp));
			}
		}

		if (sortByName) {
			std::sort(frames.begin(), frames.end(), [](xx::Shared<Frame> const& a, xx::Shared<Frame> const& b) {
				return xx::InnerNumberToFixed(a->key) < xx::InnerNumberToFixed(b->key);
				});
		}

		auto tex = xx::Make<GLTexture>(engine.LoadTexture(realTextureFileName));
		for (auto& f : frames) {
			f->tex = tex;
		}
	}

	int TP::Fill(std::string_view text, std::string_view const& rootPath) {
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
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSize.x, o.spriteSize.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSourceSize.x, o.spriteSourceSize.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find("{{"sv); i == text.npos) return __LINE__;
		if (j = text.find("}}"sv, i + 10); j == text.npos) return __LINE__;
		ReadInteger4(CutStr(text, i + 2, j - i - 2), o.textureRect.x, o.textureRect.y, o.textureRect.wh.x, o.textureRect.wh.y);
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

		if (text.starts_with("triangles<"sv)) {
			CutStr(text, 15);													// skip    triangles</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.triangles, CutStr(text, i + 8, j - i - 8));

			CutStr(text, j + 9);												// skip    </string>
			if (i = text.find('<'); i == text.npos) return __LINE__;
			CutStr(text, i + 19);												// skip    <key>vertices</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.vertices, CutStr(text, i + 8, j - i - 8));

			CutStr(text, j + 9);												// skip    </string>
			if (i = text.find('<'); i == text.npos) return __LINE__;
			CutStr(text, i + 21);												// skip    <key>verticesUV</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.verticesUV, CutStr(text, i + 8, j - i - 8));

			{
				auto len = o.vertices.size() / 2;								// flip y for ogl
				auto p = (xx::XY*)o.vertices.data();
				for (size_t k = 0; k < len; ++k) {
					p[k].y = o.spriteSize.y - p[k].y;
				}
			}

			CutStr(text, j + 9);												// skip    </string>

			if (i = text.find("<k"sv); i == text.npos) return __LINE__;
			CutStr(text, i + 5);												// skip    <key>
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
		realTextureFileName = rootPath;
		realTextureFileName.append(CutStr(text, i + 8, j - i - 8));

		return 0;
	}


	xx::Shared<Frame> const& TP::Get(std::string_view const& key) const {
		for (auto& f : frames) {
			if (f->key == key) return f;
		}
		throw std::logic_error(xx::ToString(key, " is not found in tp: ", plistFullPath));
	}

	xx::Shared<Frame> const& TP::Get(char const* const& buf, size_t const& len) const {
		return Get(std::string_view(buf, len));
	}

	size_t TP::GetToByPrefix(std::vector<xx::Shared<xx::Frame>>& fs, std::string_view const& prefix) const {
		size_t n{};
		for (auto& f : frames) {
			if (f->key.starts_with(prefix)) {
				auto s = f->key.substr(prefix.size());
				if (s[0] >= '0' && s[0] <= '9') {
					fs.push_back(f);
					++n;
				}
			}
		}
		return n;
	}

	std::vector<xx::Shared<xx::Frame>> TP::GetByPrefix(std::string_view const& prefix) const {
		std::vector<xx::Shared<xx::Frame>> fs;
		GetToByPrefix(fs, prefix);
		return fs;
	}

	void TP::GetTo(std::vector<xx::Shared<xx::Frame>>& fs, std::initializer_list<std::string_view> keys) const {
		for (auto& k : keys) {
			fs.push_back(Get(k));
		}
	}

	xx::Shared<Frame> TP::TryGet(std::string_view const& key) const {
		for (auto& f : frames) {
			if (f->key == key) return f;
		}
		return {};
	}

	std::unordered_map<std::string_view, xx::Shared<Frame>> TP::GetMapSV() const {
		std::unordered_map<std::string_view, xx::Shared<xx::Frame>> fs;
		for (auto& f : frames) {
			fs[std::string_view(f->key)] = f;
		}
		return fs;
	}

	std::unordered_map<std::string, xx::Shared<Frame>> TP::GetMapS() const {
		std::unordered_map<std::string, xx::Shared<xx::Frame>> fs;
		for (auto& f : frames) {
			fs[f->key] = f;
		}
		return fs;
	}
}
