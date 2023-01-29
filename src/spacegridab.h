#pragma once
#include "pch.h"

// space grid index system for AABB bounding box. coordinate (0, 0) at left-top, +x = right, +y = buttom
template<typename Item>
struct SpaceGridAB;

#define thisSpaceGridABItemDeriveType ((SpaceGridABItemDeriveType*)(this))

template<typename SpaceGridABItemDeriveType>
struct SpaceGridABItemCellInfo {
	SpaceGridABItemDeriveType* self{};
	size_t idx{};
	SpaceGridABItemCellInfo *prev{}, *next{};
};

// for inherit
template<typename SpaceGridABItemDeriveType>
struct SpaceGridABItem {
	using SGABCoveredCellInfo = SpaceGridABItemCellInfo<SpaceGridABItemDeriveType>;
	SpaceGridAB<SpaceGridABItemDeriveType>* _sgab{};
	xx::XY<int32_t> _sgabPos, _sgabSize, _sgabHalfSize;	// for Add & Update calc covered cells
	xx::XY<int32_t> _sgabCRIdxFrom, _sgabCRIdxTo;	// backup for Update speed up
	std::vector<SGABCoveredCellInfo> _sgabCoveredCellInfos;	// todo: change to custom single buf container ?
	size_t _sgabFlag{};	// avoid duplication when Foreach

	void SGABInit(SpaceGridAB<SpaceGridABItemDeriveType>* const& sgab) {
		assert(!_sgab);
		assert(!_sgabFlag);
		assert(_sgabCoveredCellInfos.empty());
		_sgab = sgab;
	}

	void SGABSetPosSiz(xx::XY<int32_t> const& pos, xx::XY<int32_t> const& siz) {
		assert(_sgab);
		_sgabPos = pos;
		_sgabSize = siz;
		_sgabHalfSize = siz / 2;
		assert(_sgabHalfSize.x > 0 && _sgabHalfSize.y > 0);
		assert(_sgabPos.x >= 0 && _sgabPos.x + _sgabHalfSize.x < _sgab->maxX);
		assert(_sgabPos.y >= 0 && _sgabPos.y + _sgabHalfSize.y < _sgab->maxY);
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
	using ItemCellInfo = typename Item::SGABCoveredCellInfo;
	xx::XY<int32_t> cellSize;
	int32_t numRows{}, numCols{};
	int32_t maxY{}, maxX{}, numItems{}, numActives{};	// for easy check & stat
	std::vector<ItemCellInfo*> cells;
	std::vector<Item*> results;	// tmp store Foreach items

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& cellWidth_, int32_t const& cellHeight_) {
		assert(cells.empty());
		assert(!numItems);
		assert(!numActives);

		numRows = numRows_;
		numCols = numCols_;
		cellSize.x = cellWidth_;
		cellSize.y = cellHeight_;

		maxY = cellHeight_ * numRows;
		maxX = cellWidth_ * numCols;

		cells.resize(numRows * numCols);
	}

	void Add(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.empty());

		assert(c->_sgabHalfSize.x > 0 && c->_sgabHalfSize.y > 0);
		assert(c->_sgabPos.x >= 0 && c->_sgabPos.x + c->_sgabHalfSize.x < maxX);
		assert(c->_sgabPos.y >= 0 && c->_sgabPos.y + c->_sgabHalfSize.y < maxY);

		// calc covered cells
		auto leftTopPos = c->_sgabPos - c->_sgabHalfSize;
		auto crIdxFrom = leftTopPos / cellSize;
		auto rightBottomPos = c->_sgabPos + c->_sgabHalfSize;
		auto crIdxTo = rightBottomPos / cellSize;

		assert(leftTopPos.x < rightBottomPos.x);
		assert(leftTopPos.y < rightBottomPos.y);
		assert(leftTopPos.x >= 0 && leftTopPos.y >= 0);
		assert(rightBottomPos.x < maxX && rightBottomPos.y < maxY);

		auto numCoveredCells = (crIdxTo.x - crIdxFrom.x + 1) * (crIdxTo.y - crIdxFrom.y + 1);

		// link
		auto& ccis = c->_sgabCoveredCellInfos;
		ccis.reserve(numCoveredCells);
		for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
			for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
				size_t idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				auto ci = &ccis.emplace_back(ItemCellInfo{ c, idx, nullptr, cells[idx] });
				if (cells[idx]) {
					cells[idx]->prev = ci;
				}
				cells[idx] = ci;
			}
		}

		// store idxs
		c->_sgabCRIdxFrom = crIdxFrom;
		c->_sgabCRIdxTo = crIdxTo;

		// stat
		++numItems;
	}

	void Remove(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.size());

		// unlink
		auto& ccis = c->_sgabCoveredCellInfos;
		for (auto& ci : ccis) {

			if (ci.prev) {	// isn't header
				ci.prev->next = ci.next;
				if (ci.next) {
					ci.next->prev = ci.prev;
				}
			}
			else {
				cells[ci.idx] = ci.next;
				if (ci.next) {
					ci.next->prev = {};
				}
			}
		}
		ccis.clear();

		// stat
		--numItems;
	}

	void Update(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.size());

		assert(c->_sgabHalfSize.x > 0 && c->_sgabHalfSize.y > 0);
		assert(c->_sgabPos.x >= 0 && c->_sgabPos.x + c->_sgabHalfSize.x < maxX);
		assert(c->_sgabPos.y >= 0 && c->_sgabPos.y + c->_sgabHalfSize.y < maxY);

		// calc covered cells
		auto leftTopPos = c->_sgabPos - c->_sgabHalfSize;
		auto crIdxFrom = leftTopPos / cellSize;
		auto rightBottomPos = c->_sgabPos + c->_sgabHalfSize;
		auto crIdxTo = rightBottomPos / cellSize;

		assert(leftTopPos.x < rightBottomPos.x);
		assert(leftTopPos.y < rightBottomPos.y);
		assert(leftTopPos.x >= 0 && leftTopPos.y >= 0);
		assert(rightBottomPos.x < maxX && rightBottomPos.y < maxY);

		auto numCoveredCells = (crIdxTo.x - crIdxFrom.x + 1) * (crIdxTo.y - crIdxFrom.y + 1);

		auto& ccis = c->_sgabCoveredCellInfos;
		if (numCoveredCells == ccis.size()
			&& crIdxFrom == c->_sgabCRIdxFrom
			&& crIdxTo == c->_sgabCRIdxTo) {
			return;
		}

		// unlink
		for (auto& ci : ccis) {

			if (ci.prev) {	// isn't header
				ci.prev->next = ci.next;
				if (ci.next) {
					ci.next->prev = ci.prev;
				}
			}
			else {
				cells[ci.idx] = ci.next;
				if (ci.next) {
					ci.next->prev = {};
				}
			}
		}
		ccis.clear();

		// link
		ccis.reserve(numCoveredCells);
		for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
			for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
				size_t idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				auto ci = &ccis.emplace_back(ItemCellInfo{ c, idx, nullptr, cells[idx] });
				if (cells[idx]) {
					cells[idx]->prev = ci;
				}
				cells[idx] = ci;
			}
		}

		// store idxs
		c->_sgabCRIdxFrom = crIdxFrom;
		c->_sgabCRIdxTo = crIdxTo;
	}

	int32_t CalcIndexByPosition(xx::XY<int32_t> const& pos) {
		assert(pos.x >= 0 && pos.x < maxX);
		assert(pos.y >= 0 && pos.y < maxY);
		auto crIdx = pos / cellSize;
		auto idx = crIdx.y * numCols + crIdx.x;
		assert(idx <= cells.size());
		return idx;
	}

	void ClearResults() {
		for (auto& o : results) {
			o->_sgabFlag = 0;
		}
		results.clear();
	}

	// fill items to results. need ClearResults()
	template<bool enableLimit = false, bool enableExcept = false>
	void Foreach(int32_t const& idx, int32_t* limit = nullptr, Item* const& except = nullptr) {
		assert(idx >= 0 && idx < cells.size());
		if constexpr (enableLimit) {
			assert(limit);
			if (*limit <= 0) return;
		}
		if constexpr (enableExcept) {
			assert(except);
			except->_sgabFlag = 1;
		}
		auto c = cells[idx];
		while (c) {
			if (!c->self->_sgabFlag) {
				c->self->_sgabFlag = 1;
				results.push_back(c->self);
			}
			if constexpr (enableLimit) {
				if (-- * limit == 0) break;
			}
			c = c->next;
		}
		if constexpr (enableExcept) {
			assert(except);
			except->_sgabFlag = 0;
		}
	}

	// fill items to results. need ClearResults()
	template<bool enableLimit = false, bool enableExcept = false>
	void Foreach(int32_t const& rIdx, int32_t const& cIdx, int32_t* limit = nullptr, Item* const& except = nullptr) {
		if (rIdx < 0 || rIdx >= numRows) return;
		if (cIdx < 0 || cIdx >= numCols) return;
		Foreach<enableLimit, enableExcept>(rIdx * numCols + cIdx, limit, except);
	}

	// fill items to results. need ClearResults()
	template<bool enableLimit = false, bool enableExcept = false>
	void ForeachAABB(xx::XY<int32_t> const& leftTopPos, xx::XY<int32_t> const& rightBottomPos, int32_t* limit = nullptr, Item* const& except = nullptr) {
		assert(leftTopPos.x < rightBottomPos.x);
		assert(leftTopPos.y < rightBottomPos.y);
		assert(leftTopPos.x >= 0 && leftTopPos.y >= 0);
		assert(rightBottomPos.x < maxX && rightBottomPos.y < maxY);

		// calc covered cells
		auto crIdxFrom = leftTopPos / cellSize;
		auto crIdxTo = rightBottomPos / cellSize;
		
		for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
			for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
				size_t idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				Foreach<enableLimit, enableExcept>(idx, limit, except);
				if constexpr (enableLimit) {
					assert(limit);
					if (*limit <= 0) return;
				}
			}
		}
	}
};

template<typename Item>
struct SpaceGridABCamera : Translate {
	
	SpaceGridAB<Item>* grid{};
	Size screenSize{};

	XY pos{};
	bool dirty = true;

	/*
	for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
		for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
			grid.Foreach(rIdx, cIdx, ...
	*/
	int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

	void Init(Size const& screenSize_, SpaceGridAB<Item>* const& grid_) {
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
	void SetPosition(XY const& pos) {
		this->pos = pos;
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
