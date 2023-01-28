#pragma once
#include "pch.h"

struct CircleGrid;
struct Circle {
	CircleGrid* grid{};
	Circle *prev{}, *next{};
	int32_t cellIndex{ -1 };
	int32_t radius{};
	xx::XY<int32_t> xy, xy2;

	int32_t csIndex{ -1 };	// fill when push to cs, update when swap remove
	std::unique_ptr<LineStrip> border;

	void Init(CircleGrid* const& grid_, int32_t const& x, int32_t const& y, int32_t const& r);
	void Update(Rnd& rnd);	// calc ( parallelable )
	void Update2();	// assign
	~Circle();
};

struct CircleGrid {
	int32_t numRows{}, numCols{}, maxDiameter{}, maxY{}, maxX{}, numItems{}, numActives{};
	std::vector<Circle*> cells;

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_);
	void Add(Circle* const& c);
	void Remove(Circle* const& c);
	void Update(Circle* const& c);
	int32_t CalcIndexByPosition(int32_t const& x, int32_t const& y);

	template<bool enableLimit = false, bool enableExcept = false, typename F>
	void Foreach(int32_t const& idx, F&& f, int32_t* limit = nullptr, Circle* const& except = nullptr) {
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		assert(idx >= 0 && idx < cells.size());
		auto c = cells[idx];
		while (c) {
			assert(cells[c->cellIndex]->prev == nullptr);
			assert(c->next != c);
			assert(c->prev != c);
			if constexpr (enableExcept) {
				if (c != except) {
					f(c);
				}
			} else {
				f(c);
			}
			if constexpr (enableLimit) {
				if (--*limit == 0) return;
			}
			c = c->next;
		}
	}

	template<bool enableLimit = false, bool enableExcept = false, typename F>
	void Foreach(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr, Circle* const& except = nullptr) {
		if (rIdx < 0 || rIdx >= numRows) return;
		if (cIdx < 0 || cIdx >= numCols) return;
		Foreach<enableLimit, enableExcept>(rIdx * numCols + cIdx, std::forward<F>(f), limit, except);
	}

	template<bool enableLimit = false, typename F>
	void Foreach8NeighborCells(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr) {
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

	template<bool enableLimit = false, typename F>
	void Foreach9NeighborCells(Circle* c, F&& f, int32_t* limit = nullptr) {
		Foreach<enableLimit, true>(c->cellIndex, f, limit, c);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		auto rIdx = c->cellIndex / numCols;
		auto cIdx = c->cellIndex - numCols * rIdx;
		Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
	}

	template<bool enableLimit = false, typename F>
	void Foreach9NeighborCells(int32_t const& idx, F&& f, int32_t* limit = nullptr) {
		Foreach<enableLimit>(idx, f, limit);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		auto rIdx = idx / numCols;
		auto cIdx = idx - numCols * rIdx;
		Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
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

	inline static constexpr int32_t numRows = 200, numCols = 200, minRadius = 16, maxRadius = 32;
	inline static constexpr int32_t maxDiameter = maxRadius * 2;
	inline static constexpr int32_t maxX = maxDiameter * numCols, maxY = maxDiameter * numRows;
	inline static constexpr int32_t speed = 5, speedMaxScale = 5;
	inline static constexpr int32_t foreachLimit = 15;
	inline static constexpr int32_t numRandCircles = 50000, capacity = numRandCircles * 2;
	inline static constexpr int32_t numEveryInsert = 10;
	CircleGrid grid;
	CircleGridCamera cam;
	std::vector<xx::Shared<Circle>> cs;
	Rnd rnd;
	double timePool{};
	std::vector<Circle*> tmpcs;

	XY GetMousePosInGrid();
};
