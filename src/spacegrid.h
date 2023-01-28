#pragma once
#include "pch.h"

template<typename Item>
struct SpaceGrid;

// for inherit or copy
struct SpaceGridItem {
	SpaceGrid<SpaceGridItem>* _sgrid{};
	SpaceGridItem *_sgridPrev{}, *_sgridNext{};
	int32_t _sgridIdx{ -1 };
	xx::XY<int32_t> _sgridPos;
};

template<typename Item = SpaceGridItem>
struct SpaceGrid {
	int32_t numRows{}, numCols{}, maxDiameter{}, maxY{}, maxX{}, numItems{}, numActives{};
	std::vector<Item*> cells;

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_) {
		numRows = numRows_;
		numCols = numCols_;
		maxDiameter = maxDiameter_;
		maxY = maxDiameter * numRows;
		maxX = maxDiameter * numCols;
		cells.clear();
		cells.resize(numRows * numCols);
	}

	void Add(Item* const& c) {
		assert(c);
		assert(c->_sgrid == this);
		assert(c->_sgridIdx == -1);
		assert(!c->_sgridPrev);
		assert(!c->_sgridNext);
		assert(c->_sgridPos.x >= 0 && c->_sgridPos.x < maxX);
		assert(c->_sgridPos.y >= 0 && c->_sgridPos.y < maxY);

		// calc rIdx & cIdx
		int rIdx = c->_sgridPos.y / maxDiameter, cIdx = c->_sgridPos.x / maxDiameter;
		int idx = rIdx * numCols + cIdx;
		assert(idx <= cells.size());
		assert(!cells[idx] || !cells[idx]->_sgridPrev);

		// link
		if (cells[idx]) {
			cells[idx]->_sgridPrev = c;
		}
		c->_sgridNext = cells[idx];
		c->_sgridIdx = idx;
		cells[idx] = c;
		assert(!cells[idx]->_sgridPrev);
		assert(c->_sgridNext != c);
		assert(c->_sgridPrev != c);

		// stat
		++numItems;
	}

	void Remove(Item* const& c) {
		assert(c);
		assert(c->_sgrid == this);
		assert(!c->_sgridPrev && cells[c->_sgridIdx] == c || c->_sgridPrev->_sgridNext == c && cells[c->_sgridIdx] != c);
		assert(!c->_sgridNext || c->_sgridNext->_sgridPrev == c);
		//assert(cells[c->_sgridIdx] include c);

		// unlink
		if (c->_sgridPrev) {	// isn't header
			assert(cells[c->_sgridIdx] != c);
			c->_sgridPrev->_sgridNext = c->_sgridNext;
			if (c->_sgridNext) {
				c->_sgridNext->_sgridPrev = c->_sgridPrev;
				c->_sgridNext = {};
			}
			c->_sgridPrev = {};
		} else {
			assert(cells[c->_sgridIdx] == c);
			cells[c->_sgridIdx] = c->_sgridNext;
			if (c->_sgridNext) {
				c->_sgridNext->_sgridPrev = {};
				c->_sgridNext = {};
			}
		}
		c->_sgridIdx = -1;
		assert(cells[c->_sgridIdx] != c);

		// stat
		--numItems;
	}

	void Update(Item* const& c) {
		assert(c);
		assert(c->_sgrid == this);
		assert(c->_sgridIdx > -1);
		assert(c->_sgridNext != c);
		assert(c->_sgridPrev != c);
		//assert(cells[c->_sgridIdx] include c);

		auto idx = CalcIndexByPosition(c->_sgridPos.x, c->_sgridPos.y);
		if (idx == c->_sgridIdx) return;	// no change
		assert(!cells[idx] || !cells[idx]->_sgridPrev);
		assert(!cells[c->_sgridIdx] || !cells[c->_sgridIdx]->_sgridPrev);

		// unlink
		if (c->_sgridPrev) {	// isn't header
			assert(cells[c->_sgridIdx] != c);
			c->_sgridPrev->_sgridNext = c->_sgridNext;
			if (c->_sgridNext) {
				c->_sgridNext->_sgridPrev = c->_sgridPrev;
				//c->_sgridNext = {};
			}
			//c->_sgridPrev = {};
		} else {
			assert(cells[c->_sgridIdx] == c);
			cells[c->_sgridIdx] = c->_sgridNext;
			if (c->_sgridNext) {
				c->_sgridNext->_sgridPrev = {};
				//c->_sgridNext = {};
			}
		}
		//c->_sgridIdx = -1;
		assert(cells[c->_sgridIdx] != c);
		assert(idx != c->_sgridIdx);

		// link
		if (cells[idx]) {
			cells[idx]->_sgridPrev = c;
		}
		c->_sgridPrev = {};
		c->_sgridNext = cells[idx];
		cells[idx] = c;
		c->_sgridIdx = idx;
		assert(!cells[idx]->_sgridPrev);
		assert(c->_sgridNext != c);
		assert(c->_sgridPrev != c);
	}

	int32_t CalcIndexByPosition(int32_t const& x, int32_t const& y) {
		assert(x >= 0 && x < maxX);
		assert(y >= 0 && y < maxY);
		int32_t rIdx = y / maxDiameter, cIdx = x / maxDiameter;
		auto idx = rIdx * numCols + cIdx;
		assert(idx <= cells.size());
		return idx;
	}

	template<bool enableLimit = false, bool enableExcept = false, typename F>
	void Foreach(int32_t const& idx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		assert(idx >= 0 && idx < cells.size());
		auto c = cells[idx];
		while (c) {
			assert(cells[c->_sgridIdx]->_sgridPrev == nullptr);
			assert(c->_sgridNext != c);
			assert(c->_sgridPrev != c);
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
			c = c->_sgridNext;
		}
	}

	template<bool enableLimit = false, bool enableExcept = false, typename F>
	void Foreach(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
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
	void Foreach9NeighborCells(Item* c, F&& f, int32_t* limit = nullptr) {
		Foreach<enableLimit, true>(c->_sgridIdx, f, limit, c);
		if constexpr (enableLimit) {
			if (*limit <= 0) return;
		}
		auto rIdx = c->_sgridIdx / numCols;
		auto cIdx = c->_sgridIdx - numCols * rIdx;
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

template<typename Item = SpaceGridItem>
struct SpaceGridCamera : Translate {
	
	SpaceGrid<Item>* grid{};
	Size screenSize{};

	XY pos{};
	bool dirty = true;

	/*
	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			grid.Foreach(rIdx, cIdx, [&](Item* const& c) {
	*/
	int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

	void Init(Size const& screenSize_, SpaceGrid<Item>* const& grid_) {
		grid = grid_;
		screenSize = screenSize_;
		Commit();
	}

	void SetScreenSize(Size const& wh) {
		this->screenSize = wh;
		dirty = true;
	}
	void SetScale(float const& scale) {
		this->scale = { scale, scale };
		dirty = true;
	}
	void SetPosition(XY const& _sgridPos) {
		this->pos = _sgridPos;
		dirty = true;
	}
	void SetPositionX(float const& x) {
		this->pos.x = x;
		dirty = true;
	}
	void SetPositionY(float const& y) {
		this->pos.y = y;
		dirty = true;
	}

	// call after set xxxx ...
	void Commit() {
		if (!dirty) return;
		dirty = false;

		auto d = grid->maxDiameter;
		auto fd = (float)d;
		auto halfNumRows = screenSize.h / scale.y / fd / 2;
		int32_t posRowIndex = (int32_t)pos.y / d;
		rowFrom = posRowIndex - halfNumRows;
		rowTo = posRowIndex + halfNumRows + 2;
		if (rowFrom < 0) {
			rowFrom = 0;
		}
		if (rowTo > grid->numRows) {
			rowTo = grid->numRows;
		}

		auto halfNumColumns = screenSize.w / scale.x / fd / 2;
		int32_t posColumnIndex = (int32_t)pos.x / d;
		columnFrom = posColumnIndex - halfNumColumns;
		columnTo = posColumnIndex + halfNumColumns + 2;
		if (columnFrom < 0) {
			columnFrom = 0;
		}
		if (columnTo > grid->numCols) {
			columnTo = grid->numCols;
		}

		offset = { -pos.x, pos.y };
	}

	XY GetMousePosInGrid(XY const& mousePos) {
		auto x = pos.x + mousePos.x / scale.x;
		if (x < 0) x = 0;
		else if (x >= grid->maxX) x = grid->maxX - 1;
		auto y = pos.y - mousePos.y / scale.y;
		if (y < 0) y = 0;
		else if (y >= grid->maxY) y = grid->maxY - 1;
		return { x, y };
	}
};
