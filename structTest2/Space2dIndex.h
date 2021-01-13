#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>

namespace Space2dIndex {

	struct Vec2 { int x, y; };
	struct Size { int w, h; };

	template<typename UD, int cellMappingsSize = 4>
	struct Grid {
		static const int cellMappingsSize_ = cellMappingsSize;

		struct Item {
			Vec2 pos;
			Size siz;
			int prev, next;
			size_t flag;
		};
		struct ItemEx {
			int left, right, top, bottom;
			std::array<std::pair<int, int>, cellMappingsSize> cellMappings;
			UD ud;
		};

		struct Mapping {
			int item;
			int prev, next;
		};

		Size cellSize;
		int rowCount;
		int columnCount;
	protected:

		int* cells;
		int items;

		Item* itemBuf;
		int itemBufLen;
		int itemBufCap;
		int freeItemHeader;
		int freeItemCount;
		std::vector<ItemEx> itemExBuf;

		Mapping* mappingBuf;
		int mappingBufLen;
		int mappingBufCap;
		int freeMappingHeader;
		int freeMappingCount;

	public:
		Grid(Size const& cellSize, int const& rowCount, int const& columnCount, int const& cap) {
			this->cellSize = cellSize;
			this->rowCount = rowCount;
			this->columnCount = columnCount;
			cells = (int*)malloc(sizeof(int) * rowCount * columnCount);

			itemBuf = (Item*)malloc(sizeof(Item) * cap);
			//itemExBuf = (ItemEx*)malloc(sizeof(ItemEx) * cap);
			itemExBuf.resize(cap);
			itemBufCap = cap;

			mappingBuf = (Mapping*)malloc(sizeof(Mapping) * cap * cellMappingsSize);
			mappingBufCap = cap * cellMappingsSize;

			Clear();
		}

		void Clear() {
			if constexpr (!std::is_standard_layout_v<UD>) {
				ForeachItem([this](int const& idx) {
					itemExBuf[idx].ud = UD();
					});
			}

			memset((void*)cells, -1, sizeof(int) * rowCount * columnCount);
			items = -1;

			itemBufLen = 0;
			freeItemHeader = -1;
			freeItemCount = 0;

			mappingBufLen = 0;
			freeMappingHeader = -1;
			freeMappingCount = 0;
		}

		~Grid() {
			free(cells);
			free(itemBuf);
			//free(itemExBuf);
			free(mappingBuf);
		}

		int ItemAdd(UD&& ud, Vec2 const& pos, Size const& siz) {
			//assert(ud);
			assert(siz.w >= 0 && siz.h >= 0);
			assert(((siz.w - 1) / cellSize.w + 1) * ((siz.h - 1) / cellSize.h + 1) <= cellMappingsSize);

			int idx;
			if (freeItemCount) {
				idx = freeItemHeader;
				freeItemHeader = itemBuf[idx].next;
				--freeItemCount;
			}
			else {
				if (itemBufLen == itemBufCap) {
					itemBufCap *= 2;
					itemBuf = (Item*)realloc((void*)itemBuf, sizeof(Item) * itemBufCap);
					//itemExBuf = (ItemEx*)realloc((void*)itemExBuf, sizeof(ItemEx) * itemBufCap);
					itemExBuf.resize(itemBufCap);
				}
				idx = itemBufLen;
				++itemBufLen;
			}

			auto& o = itemBuf[idx];
			auto& o2 = itemExBuf[idx];
			o.pos = pos;
			o.siz = siz;
			o.flag = 0;
			o2.ud = std::forward<UD>(ud);

			o.next = items;
			if (items != -1) {
				assert(itemBuf[items].prev == -1);
				itemBuf[items].prev = idx;
			}
			o.prev = -1;
			items = idx;

			ItemCalc<false>(idx);

			return idx;
		}

		void ItemUpdate(int const& idx, Vec2 const& pos, Size const& siz) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(itemExBuf[idx].ud);

			auto& o = itemBuf[idx];
			if (*(int64_t*)&o.pos == *(int64_t*)&pos && *(int64_t*)&o.siz == *(int64_t*)&siz) return;
			o.pos = pos;
			o.siz = siz;
			ItemCalc<true>(idx);
		}

		void ItemRemoveAt(int const& idx) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(itemBuf[idx].ud);
			auto& o = itemBuf[idx];
			if (o.prev != -1) {
				itemBuf[o.prev].next = o.next;
			}
			if (o.next != -1) {
				itemBuf[o.next].prev = o.prev;
			}
			if (items == idx) {
				items = o.next;
			}
			if constexpr (!std::is_standard_layout_v<UD>) {
				o.ud = UD();
			}
			o.next = freeItemHeader;
			ItemClearMappings(idx);
			freeItemHeader = idx;
			++freeItemCount;
		}

		Item& ItemAt(int const& idx) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			assert(itemBuf[idx].ud);
			return itemBuf[idx];
		}

		// ForeachItem([](int const& idx){  ctx.ItemAt(idx)  }); 
		template<typename F>
		void ForeachItem(F&& f) {
			int idx = items;
			while (idx != -1) {
				f(idx);
				idx = itemBuf[idx].next;
			}
		}

		int Count() const {
			return itemBufLen - freeItemCount;
		}

		// 检索 item 覆盖到的格子的邻居 items idxs
		int ItemQueryNears(int const& idx, std::vector<int>& out) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(itemExBuf[idx].ud);
			out.clear();
			auto& o = itemBuf[idx];
			auto& o2 = itemExBuf[idx];
			o.flag = 1;
			for (int n = 0; n < cellMappingsSize; ++n) {
				auto& m = o2.cellMappings[n];
				if (m.first == -1) break;
				int mIdx = cells[m.first];
				do {
					auto& m = mappingBuf[mIdx];
					if (!itemBuf[m.item].flag) {
						itemBuf[m.item].flag = 1;
						out.push_back(m.item);
					}
					mIdx = m.next;
				} while (mIdx != -1);
			}
			o.flag = 0;
			for (auto& idx : out) {
				itemBuf[idx].flag = 0;
			}
			return (int)out.size();
		}

		// todo: 按坐标, 距离检索某 圆 范围内的 items idxs
		// todo: 检索指定 Rect 范围的 items idxs
		// todo: 找离目标区域 最近的 item ? 得到范围内由近到远排列的 items list?

		void Dump() {
			int c = Count();
			std::cout << "---------------- item's count = " << c << std::endl;
			if (!c) return;

			ForeachItem([this](int const& idx) {
				auto& o = ItemAt(idx);
				std::cout << "idx = " << idx << ", ud = " << o.ud << ", x = " << o.pos.x << ", y = " << o.pos.y << ", w = " << o.siz.w << ", h = " << o.siz.h << std::endl;
				for (int n = 0; n < cellMappingsSize; ++n) {
					auto& m = o.cellMappings[n];
					if (m.first == -1) break;
					int rIdx = m.first / columnCount;
					int cIdx = m.first - rIdx * columnCount;
					std::cout << "cell = " << rIdx << "," << cIdx << std::endl;
				}
				});

			std::cout << "-------- cells: " << columnCount << " * " << rowCount << std::endl;

			for (int j = 0; j < rowCount; ++j) {
				for (int i = 0; i < columnCount; ++i) {
					int mIdx = cells[j * columnCount + i];
					bool hasItems = mIdx != -1;
					if (hasItems) {
						std::cout << j << "," << i << ": items = ";
					}
					while (mIdx != -1) {
						auto& m = mappingBuf[mIdx];
						std::cout << m.item << " ";
						mIdx = m.next;
					}
					if (hasItems) {
						std::cout << std::endl;
					}
				}
			}

			std::cout << std::endl;
		};

	protected:
		template<bool isUpdate>
		void ItemCalc(int const& idx) {
			auto& o = itemBuf[idx];
			auto& o2 = itemExBuf[idx];

			// 计算 item 覆盖到了哪些 cell
			int left = (o.pos.x - o.siz.w / 2) / cellSize.w;
			int right = (o.pos.x + o.siz.w / 2) / cellSize.w;
			int top = (o.pos.y - o.siz.h / 2) / cellSize.h;
			int bottom = (o.pos.y + o.siz.h / 2) / cellSize.h;

			// todo: 裁剪 index 范围 避免 for 内 if. 怀疑如果面积不大，不一定快。要测

			if constexpr (isUpdate) {
				// 缓存上次的 left right top bottom ? 如果判断没有变化就短路退出. 模板切换启用
				if (left == o2.left && right == o2.right && top == o2.top && bottom == o2.bottom) return;
				ItemClearMappings(idx);
			}
			o2.left = left;
			o2.right = right;
			o2.top = top;
			o2.bottom = bottom;

			int n = 0;
			for (int i = top; i <= bottom; ++i) {
				if (i < 0 || i >= rowCount) continue;
				for (int j = left; i <= right; ++i) {
					if (j < 0 || j >= columnCount) continue;
					int cIdx = i * columnCount + j;
					o2.cellMappings[n].first = cIdx;
					o2.cellMappings[n].second = MappingAdd(cIdx, idx);
					++n;
				}
			}
			if (n < cellMappingsSize - 1) {
				o2.cellMappings[n].first = -1;
			}
		}

		void ItemClearMappings(int const& idx) {
			for (int n = 0; n < cellMappingsSize; ++n) {
				auto& m = itemExBuf[idx].cellMappings[n];
				if (m.first == -1) break;
				MappingRemoveAt(m.first, m.second);
			}
		}

		int MappingAdd(int const& cellIndex, int const& itemIndex) {
			int idx;
			if (freeMappingCount) {
				idx = freeMappingHeader;
				freeMappingHeader = mappingBuf[idx].next;
				--freeMappingCount;
			}
			else {
				if (mappingBufLen == mappingBufCap) {
					mappingBufCap *= 2;
					mappingBuf = (Mapping*)realloc((void*)mappingBuf, sizeof(Mapping) * mappingBufCap);
				}
				idx = mappingBufLen;
				++mappingBufLen;
			}

			auto& m = mappingBuf[idx];
			m.item = itemIndex;
			m.prev = -1;
			m.next = cells[cellIndex];
			if (cells[cellIndex] != -1) {
				assert(mappingBuf[cells[cellIndex]].prev == -1);
				mappingBuf[cells[cellIndex]].prev = idx;
			}
			cells[cellIndex] = idx;

			return idx;
		}

		Mapping& MappingAt(int const& idx) {
			assert(idx >= 0);
			assert(idx < mappingBufLen);
			return mappingBuf[idx];
		}

		void MappingRemoveAt(int const& cellIdx, int const& idx) {
			assert(cellIdx >= 0);
			assert(cellIdx < rowCount* columnCount);
			assert(idx >= 0);
			assert(idx < mappingBufLen);
			assert(mappingBuf[idx].item != -1);
			auto& m = mappingBuf[idx];
			if (m.prev != -1) {
				mappingBuf[m.prev].next = m.next;
			}
			if (m.next != -1) {
				mappingBuf[m.next].prev = m.prev;
			}
			auto& c = cells[cellIdx];
			if (c == idx) {
				c = mappingBuf[idx].next;
			}
			m.item = -1;
			m.next = freeMappingHeader;
			freeMappingHeader = idx;
			++freeMappingCount;
		}
	};
}
