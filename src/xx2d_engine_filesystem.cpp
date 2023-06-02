#include "xx2d.h"

namespace xx {

	std::string Engine::ToSearchPath(std::string_view dir) {
		std::string s;

		dir = xx::Trim(dir);
		if (dir.empty())
			throw std::logic_error("dir is empty");

		// replace all \ to /, .\ or ./ to empty
		for (size_t i = 0, siz = dir.size(); i < siz; i++) {
			if (dir[i] == '.' && (dir[i + 1] == '\\' || dir[i + 1] == '/')) {
				++i;
				continue;
			} else if (dir[i] == '\\') {
				s.push_back('/');
			} else {
				s.push_back(dir[i]);
			}
		}
		if (s.empty())
			throw std::logic_error("dir is empty");

		// make sure / at the end
		if (s.back() != '/') {
			s.push_back('/');
		}

		return s;
	}

	bool IsAbsolute(std::string_view const& s) {
		return s[0] == '/' || (s.size() > 1 && s[1] == ':');
	}

	void Engine::SearchPathAdd(std::string_view dir, bool insertToFront) {
		auto s = ToSearchPath(dir);
		if (!IsAbsolute(s)) {
			s.insert(0, rootPath);
		}

		if (insertToFront) {
			searchPaths.insert(searchPaths.begin(), std::move(s));
		} else {
			searchPaths.push_back(std::move(s));
		}
	}

	void Engine::SearchPathReset() {
		searchPaths.clear();
		searchPaths.push_back(rootPath);
	}


	std::string Engine::GetFullPath(std::string_view fn, bool fnIsFileName) {
		// prepare
		fn = xx::Trim(fn);

		// is absolute path?
		if (IsAbsolute(fn))
			return std::string(fn);

		// foreach search path find
		for (size_t i = 0, e = searchPaths.size(); i < e; ++i) {
#ifdef __ANDROID__
			tmpPath = searchPaths[i];
			tmpPath /= fn;
#else
			tmpPath = (std::u8string&)searchPaths[i];
			tmpPath /= (std::u8string_view&)fn;
#endif
			if (std::filesystem::exists(tmpPath)) {
				if (fnIsFileName) {
					if (std::filesystem::is_regular_file(tmpPath)) goto LabReturn;
				} else {
					if (std::filesystem::is_directory(tmpPath)) goto LabReturn;
				}
			}
			continue;
		LabReturn:
#if defined(__clang__) or defined(__GNUC__)
		{
			auto tmp = tmpPath.u8string();
			return std::move((std::string&)tmp);
		}
#else
			return (std::string&&)tmpPath.u8string();
#endif
		}
		// not found
		return {};
	}


	xx::Data Engine::LoadFileDataWithFullPath(std::string_view const& fp, bool autoDecompress) {
		xx::Data d;
#ifndef __ANDROID__
		if (int r = xx::ReadAllBytes((std::u8string_view&)fp, d))
#else
		if (int r = xx::ReadAllBytes(fp, d))
#endif
			throw std::logic_error(xx::ToString("file read error. r = ", r, ", fn = ", fp));
		if (d.len == 0)
			throw std::logic_error(xx::ToString("file content is empty. fn = ", fp));
		if (autoDecompress && d.len >= 4) {
			if (d[0] == 0x28 && d[1] == 0xB5 && d[2] == 0x2F && d[3] == 0xFD) {	// zstd
				xx::Data d2;
				ZstdDecompress(d, d2);
				return d2;
			}
		}
		return d;
	}


	std::pair<xx::Data, std::string> Engine::LoadFileData(std::string_view const& fn, bool autoDecompress) {
		auto p = GetFullPath(fn);
		if (p.empty())
			throw std::logic_error("fn can't find: " + std::string(fn));
		auto d = LoadFileDataWithFullPath(p, autoDecompress);
		return { std::move(d), std::move(p) };
	}


	SupportedFileFormats Engine::DetectFileFormat(Data_r const& d) {
		std::string_view buf(d);
		if (buf.starts_with("\x1a\x45\xdf\xa3"sv)) {
			return SupportedFileFormats::Webm;
		} else if (buf.starts_with("XXMV 1.0"sv)) {
			return SupportedFileFormats::Xxmv;
		} else if (buf.starts_with("PKM 20"sv)) {
			return SupportedFileFormats::Pkm2;
		} else if (buf.starts_with("\x13\xab\xa1\x5c"sv)) {
			return SupportedFileFormats::Astc;
		} else if (buf.starts_with("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"sv)) {
			return SupportedFileFormats::Png;
		} else if (buf.starts_with("\xFF\xD8"sv)) {
			return SupportedFileFormats::Jpg;
		// ... more ?
		} else if (buf.starts_with("\x28\xB5\x2F\xFD"sv)) {
			return SupportedFileFormats::Zstd;
		} else {
			return SupportedFileFormats::Unknown;
		}
	}
}
