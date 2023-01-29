#pragma once
#include "pch.h"

// space grid index system for AABB bounding box. coordinate (0, 0) at left-top, +x = right, +y = buttom
template<typename Item>
struct SpaceGridAB;

#define thisSpaceGridABItemDeriveType ((SpaceGridABItemDeriveType*)(this))

template<typename SpaceGridABItemDeriveType>
struct SpaceGridABItemCellInfo {
	SpaceGridABItemDeriveType *self{};
	SpaceGridABItemCellInfo *prev{}, *next{};
};

// for inherit
template<typename SpaceGridABItemDeriveType>
struct SpaceGridABItem {
	using SGABCoveredCellInfo = SpaceGridABItemCellInfo<SpaceGridABItemDeriveType>;
	SpaceGridAB<SpaceGridABItemDeriveType>* _sgab{};
	xx::XY<int32_t> _sgabPos, _sgaSiz, _sgabPosLeftTop, _sgabPosRightBottom;	// for calc covered cells
	std::vector<SGABCoveredCellInfo> _sgabCoveredCellInfos;
	int32_t _sgabFlags{};	// avoid duplication when Foreach

	void SGABInit(SpaceGridAB<SpaceGridABItemDeriveType>* const& sgab) {
		assert(!_sgab);
		assert(!_sgabPrev);
		assert(!_sgabNext);
		assert(_sgabIdx == -1);
		assert(!_sgabFlags);
		assert(_sgabCoveredCellInfos.empty());
		_sgab = sgab;
	}

	void SGABSetPosSiz(xx::XY<int32_t> const& pos, xx::XY<int32_t> const& siz) {
		assert(_sgab);
		assert(siz.x > 0 && siz.y > 0);
		assert(pos.x >= 0 && pos.x + siz.x / 2 < _sgab->maxX);
		assert(pos.y >= 0 && pos.y + siz.y / 2 < _sgab->maxY);
		_sgabPos = pos;
		_sgaSiz = siz;
		auto&& hs = siz / 2;
		_sgabPosLeftTop = pos - hs;
		_sgabPosRightBottom = pos + hs;
	}

	void SGABAdd() {
		assert(_sgab);
		_sgab->Add(thisSpaceGridABItemDeriveType);
	}
	void SGABUpdate() {
		assert(_sgab);
		_sgab->Update(thisSpaceGridABItemDeriveType);
	}
	void SGABRemove() {
		assert(_sgab);
		_sgab->Remove(thisSpaceGridABItemDeriveType);
	}
};

template<typename Item>
struct SpaceGridAB {
	int32_t numRows{}, numCols{}, cellWidth{}, cellHeight{};
	int32_t maxY{}, maxX{}, numItems{}, numActives{};	// for easy check & stat
	std::vector<typename Item::SGABCoveredCellInfo*> cells;

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& cellWidth_, int32_t const& cellHeight_) {
		assert(cells.empty());
		assert(!numItems);
		assert(!numActives);

		numRows = numRows_;
		numCols = numCols_;
		cellWidth = cellWidth_;
		cellHeight = cellHeight_;

		maxY = cellHeight * numRows;
		maxX = cellWidth * numCols;

		cells.resize(numRows * numCols);
	}

	void Add(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.empty());

		assert(c->_sgaSiz.x > 0 && c->_sgaSiz.y > 0);
		assert(c->_sgabPosLeftTop.x < c->_sgabPosRightBottom.x);
		assert(c->_sgabPosLeftTop.y < c->_sgabPosRightBottom.y);
		assert(c->_sgabPosLeftTop.x >= 0 && c->_sgabPosLeftTop.y >= 0);
		assert(c->_sgabPosRightBottom.x < maxX && c->_sgabPosRightBottom.y < maxY);

		// calc covered cells
		int32_t rIdxFrom = c->_sgabPosLeftTop.y / cellHeight;
		int32_t rIdxTo = c->_sgabPosRightBottom.y / cellHeight;
		int32_t cIdxFrom = c->_sgabPosLeftTop.x / cellWidth;
		int32_t cIdxTo = c->_sgabPosRightBottom.x / cellWidth;
		auto numCoveredCells = (rIdxTo - rIdxFrom + 1) * (cIdxTo - cIdxFrom + 1);

		c->_sgabCoveredCellInfos.reserve(numCoveredCells);
		for (auto rIdx = rIdxFrom; rIdx <= rIdxTo; rIdx++) {
			for (auto cIdx = cIdxFrom; cIdx <= cIdxTo; cIdx++) {
				auto idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				auto& ci = c->_sgabCoveredCellInfos.emplace_back({ .self = c, .prev = nullptr, .next = cells[idx] });
				if (cells[idx]) {
					cells[idx]->prev = &ci;
				}
				cells[idx] = &ci;
			}
		}

		// stat
		++numItems;
	}

	//void Remove(Item* const& c) {
	//	assert(c);
	//	assert(c->_sgab == this);
	//	assert(!c->_sgabPrev && cells[c->_sgabIdx] == c || c->_sgabPrev->_sgabNext == c && cells[c->_sgabIdx] != c);
	//	assert(!c->_sgabNext || c->_sgabNext->_sgabPrev == c);
	//	//assert(cells[c->_sgabIdx] include c);

	//	// unlink
	//	if (c->_sgabPrev) {	// isn't header
	//		assert(cells[c->_sgabIdx] != c);
	//		c->_sgabPrev->_sgabNext = c->_sgabNext;
	//		if (c->_sgabNext) {
	//			c->_sgabNext->_sgabPrev = c->_sgabPrev;
	//			c->_sgabNext = {};
	//		}
	//		c->_sgabPrev = {};
	//	} else {
	//		assert(cells[c->_sgabIdx] == c);
	//		cells[c->_sgabIdx] = c->_sgabNext;
	//		if (c->_sgabNext) {
	//			c->_sgabNext->_sgabPrev = {};
	//			c->_sgabNext = {};
	//		}
	//	}
	//	c->_sgabIdx = -1;
	//	assert(cells[c->_sgabIdx] != c);

	//	// stat
	//	--numItems;
	//}

	//void Update(Item* const& c) {
	//	assert(c);
	//	assert(c->_sgab == this);
	//	assert(c->_sgabIdx > -1);
	//	assert(c->_sgabNext != c);
	//	assert(c->_sgabPrev != c);
	//	//assert(cells[c->_sgabIdx] include c);

	//	auto idx = CalcIndexByPosition(c->_sgabPos.x, c->_sgabPos.y);
	//	if (idx == c->_sgabIdx) return;	// no change
	//	assert(!cells[idx] || !cells[idx]->_sgabPrev);
	//	assert(!cells[c->_sgabIdx] || !cells[c->_sgabIdx]->_sgabPrev);

	//	// unlink
	//	if (c->_sgabPrev) {	// isn't header
	//		assert(cells[c->_sgabIdx] != c);
	//		c->_sgabPrev->_sgabNext = c->_sgabNext;
	//		if (c->_sgabNext) {
	//			c->_sgabNext->_sgabPrev = c->_sgabPrev;
	//			//c->_sgabNext = {};
	//		}
	//		//c->_sgabPrev = {};
	//	} else {
	//		assert(cells[c->_sgabIdx] == c);
	//		cells[c->_sgabIdx] = c->_sgabNext;
	//		if (c->_sgabNext) {
	//			c->_sgabNext->_sgabPrev = {};
	//			//c->_sgabNext = {};
	//		}
	//	}
	//	//c->_sgabIdx = -1;
	//	assert(cells[c->_sgabIdx] != c);
	//	assert(idx != c->_sgabIdx);

	//	// link
	//	if (cells[idx]) {
	//		cells[idx]->_sgabPrev = c;
	//	}
	//	c->_sgabPrev = {};
	//	c->_sgabNext = cells[idx];
	//	cells[idx] = c;
	//	c->_sgabIdx = idx;
	//	assert(!cells[idx]->_sgabPrev);
	//	assert(c->_sgabNext != c);
	//	assert(c->_sgabPrev != c);
	//}

	//int32_t CalcIndexByPosition(int32_t const& x, int32_t const& y) {
	//	assert(x >= 0 && x < maxX);
	//	assert(y >= 0 && y < maxY);
	//	int32_t rIdx = y / maxDiameter, cIdx = x / maxDiameter;
	//	auto idx = rIdx * numCols + cIdx;
	//	assert(idx <= cells.size());
	//	return idx;
	//}

	//template<bool enableLimit = false, bool enableExcept = false, typename F>
	//void Foreach(int32_t const& idx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
	//	if constexpr (enableLimit) {
	//		assert(limit);
	//		if (*limit <= 0) return;
	//	}
	//	assert(idx >= 0 && idx < cells.size());
	//	auto c = cells[idx];
	//	while (c) {
	//		assert(cells[c->_sgabIdx]->_sgabPrev == nullptr);
	//		assert(c->_sgabNext != c);
	//		assert(c->_sgabPrev != c);
	//		if constexpr (enableExcept) {
	//			if (c != except) {
	//				f(c);
	//			}
	//		} else {
	//			f(c);
	//		}
	//		if constexpr (enableLimit) {
	//			if (--*limit == 0) return;
	//		}
	//		c = c->_sgabNext;
	//	}
	//}

	//template<bool enableLimit = false, bool enableExcept = false, typename F>
	//void Foreach(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
	//	if (rIdx < 0 || rIdx >= numRows) return;
	//	if (cIdx < 0 || cIdx >= numCols) return;
	//	Foreach<enableLimit, enableExcept>(rIdx * numCols + cIdx, std::forward<F>(f), limit, except);
	//}

	//template<bool enableLimit = false, typename F>
	//void Foreach8NeighborCells(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr) {
	//	Foreach<enableLimit>(rIdx + 1, cIdx, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx - 1, cIdx, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx, cIdx + 1, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx, cIdx - 1, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx + 1, cIdx + 1, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx + 1, cIdx - 1, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx - 1, cIdx + 1, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	Foreach<enableLimit>(rIdx - 1, cIdx - 1, f, limit);
	//}

	//template<bool enableLimit = false, typename F>
	//void Foreach9NeighborCells(Item* c, F&& f, int32_t* limit = nullptr) {
	//	Foreach<enableLimit, true>(c->_sgabIdx, f, limit, c);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	auto rIdx = c->_sgabIdx / numCols;
	//	auto cIdx = c->_sgabIdx - numCols * rIdx;
	//	Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
	//}

	//template<bool enableLimit = false, typename F>
	//void Foreach9NeighborCells(int32_t const& idx, F&& f, int32_t* limit = nullptr) {
	//	Foreach<enableLimit>(idx, f, limit);
	//	if constexpr (enableLimit) {
	//		if (*limit <= 0) return;
	//	}
	//	auto rIdx = idx / numCols;
	//	auto cIdx = idx - numCols * rIdx;
	//	Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
	//}
};

//template<typename Item>
//struct SpaceGridABCamera : Translate {
//	
//	SpaceGridAB<Item>* grid{};
//	Size screenSize{};
//
//	XY pos{};
//	bool dirty = true;
//
//	/*
//	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
//		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
//			grid.Foreach(rIdx, cIdx, [&](Item* const& c) {
//	*/
//	int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;
//
//	void Init(Size const& screenSize_, SpaceGridAB<Item>* const& grid_) {
//		grid = grid_;
//		screenSize = screenSize_;
//		Commit();
//	}
//
//	void SetScreenSize(Size const& wh) {
//		this->screenSize = wh;
//		dirty = true;
//	}
//	void SetScale(float const& scale) {
//		this->scale = { scale, scale };
//		dirty = true;
//	}
//	void SetPosition(XY const& _sgabPos) {
//		this->pos = _sgabPos;
//		dirty = true;
//	}
//	void SetPositionX(float const& x) {
//		this->pos.x = x;
//		dirty = true;
//	}
//	void SetPositionY(float const& y) {
//		this->pos.y = y;
//		dirty = true;
//	}
//
//	// call after set xxxx ...
//	void Commit() {
//		if (!dirty) return;
//		dirty = false;
//
//		auto d = grid->maxDiameter;
//		auto fd = (float)d;
//		auto halfNumRows = screenSize.h / scale.y / fd / 2;
//		int32_t posRowIndex = (int32_t)pos.y / d;
//		rowFrom = posRowIndex - halfNumRows;
//		rowTo = posRowIndex + halfNumRows + 2;
//		if (rowFrom < 0) {
//			rowFrom = 0;
//		}
//		if (rowTo > grid->numRows) {
//			rowTo = grid->numRows;
//		}
//
//		auto halfNumColumns = screenSize.w / scale.x / fd / 2;
//		int32_t posColumnIndex = (int32_t)pos.x / d;
//		columnFrom = posColumnIndex - halfNumColumns;
//		columnTo = posColumnIndex + halfNumColumns + 2;
//		if (columnFrom < 0) {
//			columnFrom = 0;
//		}
//		if (columnTo > grid->numCols) {
//			columnTo = grid->numCols;
//		}
//
//		offset = { -pos.x, pos.y };
//	}
//
//	XY GetMousePosInGrid(XY const& mousePos) {
//		auto x = pos.x + mousePos.x / scale.x;
//		if (x < 0) x = 0;
//		else if (x >= grid->maxX) x = grid->maxX - 1;
//		auto y = pos.y - mousePos.y / scale.y;
//		if (y < 0) y = 0;
//		else if (y >= grid->maxY) y = grid->maxY - 1;
//		return { x, y };
//	}
//};
