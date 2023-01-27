#pragma once
#include "pch.h"

struct CircleGrid;
struct Circle {
	Circle *prev{}, *next{};				// fill by grid
	int32_t cellIndex{ -1 };				// fill by grid

	//CircleGrid* grid{};
	int32_t x{}, y{};						// read by grid
	int32_t radius{};

	std::unique_ptr<LineStrip> border;
};

struct CircleGrid {
	int32_t numRows{}, numCols{}, maxDiameter{}, maxY{}, maxX{}, numItems{};
	std::vector<Circle*> cells;

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_);
	void Add(Circle* const& c);
	void Remove(Circle* const& c);
	void Update(Circle* const& c);

	template<bool enableLimit = false, typename F>
	void Foreach(int32_t const& idx, F&& f, int32_t* limit = nullptr) {
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		assert(idx >= 0 && idx < cells.size());
		auto c = cells[idx];
		while (c) {
			f(c);
			if constexpr (enableLimit) {
				if (--*limit == 0) return;
			}
			c = c->next;
		}
	}

	template<bool enableLimit = false, typename F>
	void Foreach(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr) {
		if (rIdx < 0 || rIdx >= numRows) return;
		if (cIdx < 0 || cIdx >= numCols) return;
		Foreach<enableLimit>(rIdx * numCols + cIdx, std::forward<F>(f), limit);
	}

	template<bool enableLimit = false, typename F>
	void ForeachNeighbors(Circle* tar, F&& f, int32_t* limit = nullptr) {
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		assert(tar);
		//assert(cells[t->cellIndex] include t);

		Circle* c;

		// skip tar
		if (tar->prev) {
			tar->prev->next = tar->next;	// unlink
			c = cells[tar->cellIndex];
		} else {
			c = tar->next;
		}

		// foreach
		while (c) {
			f(c);
			if constexpr (enableLimit) {
				if (--*limit == 0) return;
			}
			c = c->next;
		}

		if (tar->prev) {
			tar->prev->next = tar;	// link
		}
	}

	template<bool enableLimit = false, typename F>
	void Foreach9Neighbors(Circle* c, F&& f, int32_t* limit = nullptr) {
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		ForeachNeighbors<enableLimit>(c, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		auto rIdx = c->cellIndex / numCols;
		auto cIdx = c->cellIndex - numCols * rIdx;
		Foreach<enableLimit>(rIdx + 1, cIdx, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx - 1, cIdx, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx, cIdx + 1, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx, cIdx - 1, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx + 1, cIdx + 1, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx + 1, cIdx - 1, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx - 1, cIdx + 1, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		Foreach<enableLimit>(rIdx - 1, cIdx - 1, f, limit);
	}
};

struct CircleGridCamera : Translate {
	
	CircleGrid* cg;		// fill by init
	Size screenSize{};	// fill by init

	XY pos{};
	bool dirty = true;

	/*
	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			cg.Foreach(rIdx, cIdx, [&](Circle* const& c) {
	*/
	int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

	void Init(Size const& screenSize, CircleGrid* const& cg);

	void SetScreenSize(Size const& wh);
	void SetScale(float const& scale);
	void SetPosition(XY const& xy);
	void SetPositionX(float const& x);
	void SetPositionY(float const& y);

	// call after set xxxx ...
	void Commit();
};


struct Logic4 : LogicBase {
	void Init(Logic* eg) override;
	int32_t Update() override;

	inline static constexpr int32_t numRows = 100, numCols = 100, minRadius = 16, maxRadius = 32;
	inline static constexpr int32_t maxDiameter = maxRadius * 2;
	inline static constexpr int32_t maxX = maxDiameter * numCols, maxY = maxDiameter * numRows;
	CircleGrid cg;
	CircleGridCamera cam;
	std::vector<Circle> cs;
	Rnd rnd;
	double timePool{};
};
