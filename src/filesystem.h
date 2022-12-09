#pragma once
#include "pch.h"

struct FileSystem {
	std::vector<std::string> searchPaths;
	void SearchPathAdd(std::string_view const& dir);
	void SearchPathClear();
};
