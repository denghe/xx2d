#pragma once
#include "xx2d.h"

// data store file json structs
namespace MovePathStore {
	struct Point {
		float x{}, y{}, tension{};
		uint32_t numSegments{};
	};
	struct Line {
		std::string name;
		bool isLoop{};
		std::vector<Point> points;
	};
	struct Group {
		std::string name;
		std::vector<std::string> lineNames;	// old name: lineIndexs
	};
	struct Data {
		uint32_t designWidth{}, designHeight{}, safeLength{};
		std::vector<Line> lines;
		std::vector<Group> groups;
	};
}
