#include "pch.h"

void Engine::SearchPathAdd(std::string_view dir) {
	// prepare
	dir = xx::Trim(dir);
	if (dir.empty())
		throw std::logic_error("dir is empty");

	// replace all \ to /, .\ or ./ to empty
	auto& s = searchPaths.emplace_back();
	for (size_t i = 0, siz = dir.size(); i < siz; i++) {
		if (dir[i] == '.' && (dir[i + 1] == '\\' || dir[i + 1] == '/')) {
			++i;
			continue;
		}
		else if (dir[i] == '\\') {
			s.push_back('/');
		}
		else {
			s.push_back(dir[i]);
		}
	}
	if (s.empty())
		throw std::logic_error("dir is empty");

	// make sure / at the end
	if (s.back() != '/') {
		s.push_back('/');
	}
}


void Engine::SearchPathReset() {
	searchPaths.clear();
	SearchPathAdd(std::filesystem::absolute("./").string());
}


std::string Engine::GetFullPath(std::string_view fn) {
	// prepare
	fn = xx::Trim(fn);

	// is absolute path?
	if (fn[0] == '/' || (fn.size() > 1 && fn[1] == ':'))
		return std::string(fn);

	// search file. order by search paths desc
	for (ptrdiff_t i = searchPaths.size() - 1; i >= 0; --i) {
		tmpPath = searchPaths[i];
		tmpPath /= fn;
		if (std::filesystem::exists(tmpPath) && std::filesystem::is_regular_file(tmpPath))
			return tmpPath.string();
	}

	// not found
	return {};
}


xx::Data Engine::ReadAllBytesWithFullPath(std::string_view const& fp, bool autoUnzip) {
	xx::Data d;
	if (int r = xx::ReadAllBytes(fp, d)) throw std::logic_error(xx::ToString("file read error. r = ", r, ", fn = ", p));
	if(d.len == 0) throw std::logic_error(xx::ToString("file content is empty. fn = ", p));
	if (autoUnzip) {
		throw std::logic_error(xx::ToString("todo: file content auto unzip. fn = ", p));
	}
	return d;
}


std::pair<xx::Data, std::string> Engine::ReadAllBytes(std::string_view const& fn, bool autoUnzip) {
	auto p = GetFullPath(fn);
	if (p.empty()) throw std::logic_error("fn can't find: " + std::string(fn));
	auto d = ReadAllBytesWithFullPath(p, autoUnzip);
	return { std::move(d), std::move(p)};
}
