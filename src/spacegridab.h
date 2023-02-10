#pragma once
#include "pch.h"

namespace xx {

	// space grid index system for AABB bounding box. coordinate (0, 0) at left-top, +x = right, +y = buttom
	template<typename Item>
	struct SpaceGridAB;

#define thisSpaceGridABItemDeriveType ((SpaceGridABItemDeriveType*)(this))

	template<typename SpaceGridABItemDeriveType>
	struct SpaceGridABItemCellInfo {
		SpaceGridABItemDeriveType* self{};
		size_t idx{};
		SpaceGridABItemCellInfo* prev{}, * next{};
	};

	// for inherit
	template<typename SpaceGridABItemDeriveType>
	struct SpaceGridABItem {
		using SGABCoveredCellInfo = SpaceGridABItemCellInfo<SpaceGridABItemDeriveType>;
		SpaceGridAB<SpaceGridABItemDeriveType>* _sgab{};
		Pos<int32_t> _sgabPos, _sgabMin, _sgabMax;	// for Add & Update calc covered cells
		Pos<int32_t> _sgabCRIdxFrom, _sgabCRIdxTo;	// backup for Update speed up
		std::vector<SGABCoveredCellInfo> _sgabCoveredCellInfos;	// todo: change to custom single buf container ?
		size_t _sgabFlag{};	// avoid duplication when Foreach

		void SGABInit(SpaceGridAB<SpaceGridABItemDeriveType>* const& sgab) {
			assert(!_sgab);
			assert(!_sgabFlag);
			assert(_sgabCoveredCellInfos.empty());
			_sgab = sgab;
		}

		void SGABSetPosSiz(Pos<int32_t> const& pos, Pos<int32_t> const& siz) {
			assert(_sgab);
			_sgabPos = pos;
			auto hs = siz / 2;
			_sgabMin = pos - hs;
			_sgabMax = pos + hs;
			assert(_sgabMin.x < _sgabMax.x);
			assert(_sgabMin.y < _sgabMax.y);
			assert(_sgabMin.x >= 0 && _sgabMin.y >= 0);
			assert(_sgabMax.x < _sgab->maxX&& _sgabMax.y < _sgab->maxY);
		}

		bool SGABCheckIntersects(Pos<int32_t> const& minXY, Pos<int32_t> const& maxXY) {
			return !(maxXY.x < _sgabMin.x || _sgabMax.x < minXY.x || maxXY.y < _sgabMin.y || _sgabMax.y < minXY.y);
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
		Pos<int32_t> cellSize;
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

			// calc covered cells
			auto crIdxFrom = c->_sgabMin / cellSize;
			auto crIdxTo = c->_sgabMax / cellSize;
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
				} else {
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

			// calc covered cells
			auto crIdxFrom = c->_sgabMin / cellSize;
			auto crIdxTo = c->_sgabMax / cellSize;
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
				} else {
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

		int32_t CalcIndexByPosition(Pos<int32_t> const& pos) {
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
		void ForeachAABB(Pos<int32_t> const& minXY, Pos<int32_t> const& maxXY, int32_t* limit = nullptr, Item* const& except = nullptr) {
			assert(minXY.x < maxXY.x);
			assert(minXY.y < maxXY.y);
			assert(minXY.x >= 0 && minXY.y >= 0);
			assert(maxXY.x < maxX&& maxXY.y < maxY);

			// calc covered cells
			auto crIdxFrom = minXY / cellSize;
			auto crIdxTo = maxXY / cellSize;

			// except set flag
			if constexpr (enableExcept) {
				assert(except);
				except->_sgabFlag = 1;
			}

			if (crIdxFrom.x == crIdxTo.x || crIdxFrom.y == crIdxTo.y) {
				// 1-2 row, 1-2 col: do full rect check
				for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
					for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (s->SGABCheckIntersects(minXY, maxXY)) {
								if (!s->_sgabFlag) {
									s->_sgabFlag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (-- * limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}
			} else {
				// first row: check AB
				auto rIdx = crIdxFrom.y;

				// first cell: check top left AB
				auto cIdx = crIdxFrom.x;
				auto c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					if (s->_sgabMax.x > minXY.x && s->_sgabMax.y > minXY.y) {
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
							results.push_back(s);
						}
						if constexpr (enableLimit) {
							if (-- * limit == 0) break;
						}
					}
					c = c->next;
				}

				// middle cells: check top AB
				for (++cIdx; cIdx < crIdxTo.x; cIdx++) {
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMax.y > minXY.y) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (-- * limit == 0) break;
							}
						}
						c = c->next;
					}
				}

				// last cell: check top right AB
				if (cIdx == crIdxTo.x) {
					auto c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMin.x < maxXY.x && s->_sgabMax.y > minXY.y) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (-- * limit == 0) break;
							}
						}
						c = c->next;
					}
				}

				// middle rows: first & last col check AB
				for (++rIdx; rIdx < crIdxTo.y; rIdx++) {

					// first cell: check left AB
					cIdx = crIdxFrom.x;
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMax.x > minXY.x) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (-- * limit == 0) break;
							}
						}
						c = c->next;
					}

					// middle cols: no check
					for (; cIdx < crIdxTo.x; cIdx++) {
						c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (-- * limit == 0) break;
							}
							c = c->next;
						}
					}

					// last cell: check right AB
					if (cIdx == crIdxTo.x) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (s->_sgabMin.x < maxXY.x) {
								if (!s->_sgabFlag) {
									s->_sgabFlag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (-- * limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}

				// last row: check AB
				if (rIdx == crIdxTo.y) {

					// first cell: check left bottom AB
					cIdx = crIdxFrom.x;
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMax.x > minXY.x && s->_sgabMin.y < maxXY.y) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (-- * limit == 0) break;
							}
						}
						c = c->next;
					}

					// middle cells: check bottom AB
					for (++cIdx; cIdx < crIdxTo.x; cIdx++) {
						c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (s->_sgabMin.y < maxXY.y) {
								if (!s->_sgabFlag) {
									s->_sgabFlag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (-- * limit == 0) break;
								}
							}
							c = c->next;
						}
					}

					// last cell: check right bottom AB
					if (cIdx == crIdxTo.x) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (s->_sgabMin.x < maxXY.x && s->_sgabMin.y < maxXY.y) {
								if (!s->_sgabFlag) {
									s->_sgabFlag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (-- * limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}
			}

			// except clear flag
			if constexpr (enableExcept) {
				assert(except);
				except->_sgabFlag = 0;
			}

		}
	};

	// todo: test & use
	template<typename Item>
	struct SpaceGridABCamera {

		SpaceGridAB<Item>* grid{};
		XY screenSize{};

		XY pos{}, scale{ 1,1 };
		AffineTransform at;
		bool dirty = true;

		/*
		for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
			for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
				grid.Foreach(rIdx, cIdx, ...
		*/
		int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

		void Init(XY const& screenSize_, SpaceGridAB<Item>* const& grid_) {
			grid = grid_;
			screenSize = screenSize_;
			Commit();
		}

		void SetScreenSize(XY const& wh) {
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

			at = at.MakePosScale(XY{ -pos.x, pos.y } *scale, scale);
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

}
