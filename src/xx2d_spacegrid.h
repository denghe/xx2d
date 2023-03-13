#pragma once
#include "xx2d.h"

namespace xx {

	// tips: put it in front of other members for destruct life cycle

	// space grid index system for circle
	template<typename Item>
	struct SpaceGridC;

#define thisSpaceGridCItemDeriveType ((SpaceGridCItemDeriveType*)(this))

	// for inherit
	template<typename SpaceGridCItemDeriveType>
	struct SpaceGridCItem {
		SpaceGridC<SpaceGridCItemDeriveType>* _sgc{};
		SpaceGridCItemDeriveType* _sgcPrev{}, * _sgcNext{};
		int32_t _sgcIdx{ -1 };
		Pos<int32_t> _sgcPos;

		void SGCInit(SpaceGridC<SpaceGridCItemDeriveType>* const& sgc) {
			assert(!_sgc);
			_sgc = sgc;
		}

		void SGCSetPos(Pos<int32_t> const& pos) {
			assert(_sgc);
			assert(_sgcPos.x >= 0 && _sgcPos.x < _sgc->maxX);
			assert(_sgcPos.y >= 0 && _sgcPos.y < _sgc->maxY);
			_sgcPos = pos;
		}

		void SGCAdd() {
			assert(_sgc);
			_sgc->Add(thisSpaceGridCItemDeriveType);
		}
		void SGCUpdate() {
			assert(_sgc);
			_sgc->Update(thisSpaceGridCItemDeriveType);
		}
		void SGCRemove() {
			assert(_sgc);
			_sgc->Remove(thisSpaceGridCItemDeriveType);
		}

		void SGCInit(SpaceGridC<SpaceGridCItemDeriveType>* const& sgc, Pos<int32_t> const& pos) {
			assert(!_sgc);
			_sgc = sgc;
			SGCSetPos(pos);
			SGCAdd();
		}
		void SGCUpdate(Pos<int32_t> const& pos) {
			SGCSetPos(pos);
			SGCUpdate();
		}
		void SGCTryRemove() {
			if (_sgc) {
				SGCRemove();
				_sgc = {};
			}
		}
	};

	template<typename Item>
	struct SpaceGridC {
		int32_t numRows{}, numCols{}, maxDiameter{};
		int32_t maxY{}, maxX{}, maxY1{}, maxX1{}, numItems{}, numActives{};	// for easy check & stat
		std::vector<Item*> cells;

		int32_t PosToIndex(xx::Pos<> const& p) {
			auto rcIdx = p / maxDiameter;
			return rcIdx.y * numCols + rcIdx.x;
		}

		void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_) {
			numRows = numRows_;
			numCols = numCols_;
			maxDiameter = maxDiameter_;
			maxY = maxDiameter * numRows;
			maxX = maxDiameter * numCols;
			maxY1 = maxY - 1;
			maxX1 = maxX - 1;
			cells.clear();
			cells.resize(numRows * numCols);
		}

		void Add(Item* const& c) {
			assert(c);
			assert(c->_sgc == this);
			assert(c->_sgcIdx == -1);
			assert(!c->_sgcPrev);
			assert(!c->_sgcNext);
			assert(c->_sgcPos.x >= 0 && c->_sgcPos.x < maxX);
			assert(c->_sgcPos.y >= 0 && c->_sgcPos.y < maxY);

			// calc rIdx & cIdx
			int rIdx = c->_sgcPos.y / maxDiameter, cIdx = c->_sgcPos.x / maxDiameter;
			int idx = rIdx * numCols + cIdx;
			assert(idx <= cells.size());
			assert(!cells[idx] || !cells[idx]->_sgcPrev);

			// link
			if (cells[idx]) {
				cells[idx]->_sgcPrev = c;
			}
			c->_sgcNext = cells[idx];
			c->_sgcIdx = idx;
			cells[idx] = c;
			assert(!cells[idx]->_sgcPrev);
			assert(c->_sgcNext != c);
			assert(c->_sgcPrev != c);

			// stat
			++numItems;
		}

		void Remove(Item* const& c) {
			assert(c);
			assert(c->_sgc == this);
			assert(!c->_sgcPrev && cells[c->_sgcIdx] == c || c->_sgcPrev->_sgcNext == c && cells[c->_sgcIdx] != c);
			assert(!c->_sgcNext || c->_sgcNext->_sgcPrev == c);
			//assert(cells[c->_sgcIdx] include c);

			// unlink
			if (c->_sgcPrev) {	// isn't header
				assert(cells[c->_sgcIdx] != c);
				c->_sgcPrev->_sgcNext = c->_sgcNext;
				if (c->_sgcNext) {
					c->_sgcNext->_sgcPrev = c->_sgcPrev;
					c->_sgcNext = {};
				}
				c->_sgcPrev = {};
			} else {
				assert(cells[c->_sgcIdx] == c);
				cells[c->_sgcIdx] = c->_sgcNext;
				if (c->_sgcNext) {
					c->_sgcNext->_sgcPrev = {};
					c->_sgcNext = {};
				}
			}
			c->_sgcIdx = -1;
			assert(cells[c->_sgcIdx] != c);

			// stat
			--numItems;
		}

		void Update(Item* const& c) {
			assert(c);
			assert(c->_sgc == this);
			assert(c->_sgcIdx > -1);
			assert(c->_sgcNext != c);
			assert(c->_sgcPrev != c);
			//assert(cells[c->_sgcIdx] include c);

			auto idx = CalcIndexByPosition(c->_sgcPos.x, c->_sgcPos.y);
			if (idx == c->_sgcIdx) return;	// no change
			assert(!cells[idx] || !cells[idx]->_sgcPrev);
			assert(!cells[c->_sgcIdx] || !cells[c->_sgcIdx]->_sgcPrev);

			// unlink
			if (c->_sgcPrev) {	// isn't header
				assert(cells[c->_sgcIdx] != c);
				c->_sgcPrev->_sgcNext = c->_sgcNext;
				if (c->_sgcNext) {
					c->_sgcNext->_sgcPrev = c->_sgcPrev;
					//c->_sgcNext = {};
				}
				//c->_sgcPrev = {};
			} else {
				assert(cells[c->_sgcIdx] == c);
				cells[c->_sgcIdx] = c->_sgcNext;
				if (c->_sgcNext) {
					c->_sgcNext->_sgcPrev = {};
					//c->_sgcNext = {};
				}
			}
			//c->_sgcIdx = -1;
			assert(cells[c->_sgcIdx] != c);
			assert(idx != c->_sgcIdx);

			// link
			if (cells[idx]) {
				cells[idx]->_sgcPrev = c;
			}
			c->_sgcPrev = {};
			c->_sgcNext = cells[idx];
			cells[idx] = c;
			c->_sgcIdx = idx;
			assert(!cells[idx]->_sgcPrev);
			assert(c->_sgcNext != c);
			assert(c->_sgcPrev != c);
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
				assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
				assert(c->_sgcNext != c);
				assert(c->_sgcPrev != c);
				if constexpr (enableExcept) {
					if (c != except) {
						f(c);
					}
				} else {
					f(c);
				}
				if constexpr (enableLimit) {
					if (-- * limit <= 0) return;
				}
				c = c->_sgcNext;
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
			Foreach<enableLimit, true>(c->_sgcIdx, f, limit, c);
			if constexpr (enableLimit) {
				if (*limit <= 0) return;
			}
			auto rIdx = c->_sgcIdx / numCols;
			auto cIdx = c->_sgcIdx - numCols * rIdx;
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

	template<typename Item>
	struct SpaceGridCCamera {

		SpaceGridC<Item>* grid{};
		XY screenSize{};

		XY pos{}, scale{ 1,1 };
		AffineTransform at;
		bool dirty = true;

		/*
		for (auto rIdx = cam.rowFrom; rIdx < cam.rowTo; ++rIdx) {
			for (auto cIdx = cam.columnFrom; cIdx < cam.columnTo; ++cIdx) {
				grid.Foreach(rIdx, cIdx, [&](Item* const& c) {
		*/
		int32_t rowFrom = 0, rowTo = 0, columnFrom = 0, columnTo = 0;

		void Init(XY const& screenSize_, SpaceGridC<Item>* const& grid_) {
			grid = grid_;
			screenSize = screenSize_;
			Commit();
		}

		void SetScreenSize(XY const& wh) {
			this->screenSize = wh;
			dirty = true;
		}
		void SetScale(float const& s) {
			this->scale = { s, s };
			dirty = true;
		}
		void SetPosition(XY const& p) {
			this->pos = p;
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
			auto halfNumRows = screenSize.y / scale.y / fd / 2;
			int32_t posRowIndex = (int32_t)pos.y / d;
			rowFrom = posRowIndex - halfNumRows;
			rowTo = posRowIndex + halfNumRows + 2;
			if (rowFrom < 0) {
				rowFrom = 0;
			}
			if (rowTo > grid->numRows) {
				rowTo = grid->numRows;
			}

			auto halfNumColumns = screenSize.x / scale.x / fd / 2;
			int32_t posColumnIndex = (int32_t)pos.x / d;
			columnFrom = posColumnIndex - halfNumColumns;
			columnTo = posColumnIndex + halfNumColumns + 2;
			if (columnFrom < 0) {
				columnFrom = 0;
			}
			if (columnTo > grid->numCols) {
				columnTo = grid->numCols;
			}

			at = at.MakePosScale(XY{ -pos.x, pos.y } * scale, scale);
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
