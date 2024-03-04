#include "main.h"

// near box ombine algorithm

struct Pos {
	int x, y;
};
struct Rect : Pos {
	int w, h;
};

int main() {
	constexpr int numRows = 5, numCols = 5;

	int data[numRows][numCols] = {
		{1,1,1,0,1},
		{1,1,0,0,1},
		{0,0,0,0,1},
		{1,1,1,0,1},
		{1,1,1,0,1},
	};

	xx::Shared<Rect> rects[numRows][numCols];

	for (int y = 0; y < numRows; ++y) {
		for (int x = 0; x < numCols; ++x) {
			if (data[y][x]) {
				rects[y][x] = xx::MakeShared<Rect>(Rect{ x, y, 1, 1 });
			}
		}
	}

	for (int y = 0; y < numRows; ++y) {
		for (int x = 0; x < numCols - 1; ++x) {
			auto& a = rects[y][x];
			auto& b = rects[y][x + 1];
			if (!a || !b || a == b) continue;
			a->w++;
			b = a;
		}
	}

	for (int y = 0; y < numRows - 1; ++y) {
		for (int x = 0; x < numCols; ++x) {
			auto& a = rects[y][x];
			auto& b = rects[y + 1][x];
			if (!a || !b || a == b) continue;
			if (a->x == b->x && a->w == b->w) {
				a->h++;
				for (int i = b->x; i < b->x + b->w; ++i) {
					rects[y + 1][i] = a;
				}
			}
		}
	}

	std::unordered_set<xx::Shared<Rect>> set;
	for (int y = 0; y < numRows; ++y) {
		for (int x = 0; x < numCols; ++x) {
			set.insert(rects[y][x]);
		}
	}
	for (auto& rect : set) {
		if (!rect) continue;
		std::cout << "x,y = " << rect->x << " " << rect->y << " w,h = " << rect->w << " " << rect->h << std::endl;
	}

	return 0;
}
