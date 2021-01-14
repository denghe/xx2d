#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>

// 格子空间 点坐标 索引系统
// 针对 Cell 尺寸大于所有 Item 的情况, 将空间索引系统简化为 中心点 数据管理. Item 和 Cell 1对1, Cell 和 Item 1对多 不重复
// Cell 即 Item 链表头 ( 类似 hash map 的 bucket )
// UD 和 Item 分离存储，下标一致, 确保内存集中分布提升查询效率
// 如果真出现 Item 大于 Cell 尺寸的情况, 可以创建多套 Grid, 分级管理( 类似4叉树的感觉 ), 合并查询结果

namespace Space2dPointIndex {

	template<typename UD>
	struct Grid {
		struct Item {
			//float x, y;
			int prev, next;
			int cellIdx;
		};

		float cellWidth, cellHeight;
		int rowCount;
		int columnCount;

	protected:
		int* cells;
		int outside;

		Item* itemBuf;
		int itemBufLen;
		int itemBufCap;
		int freeItemHeader;
		int freeItemCount;

		std::vector<UD> udBuf;

	public:
		Grid(float const& cellWidth, float const& cellHeight, int const& rowCount, int const& columnCount, int const& cap)
			: cellWidth(cellWidth)
			, cellHeight(cellHeight)
			, rowCount(rowCount)
			, columnCount(columnCount)
			, itemBufCap(cap)
		{
			cells = (int*)malloc(sizeof(int) * rowCount * columnCount);

			itemBuf = (Item*)malloc(sizeof(Item) * cap);
			itemBufCap = cap;

			udBuf.resize(cap);

			Clear<true>();
		}

		template<bool isCtor = false>
		void Clear() {
			if constexpr (!isCtor && !std::is_standard_layout_v<UD>) {
				ForeachItemIndex([this](int const& idx) {
					udBuf[idx] = UD();
				});
			}
			memset((void*)cells, -1, sizeof(int) * rowCount * columnCount);
			outside = -1;

			itemBufLen = 0;
			freeItemHeader = -1;
			freeItemCount = 0;
		}

		~Grid() {
			free(cells);
			free(itemBuf);
		}

		int ItemAdd(UD&& ud, float const& x, float const& y) {
			//assert(ud);
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
					udBuf.resize(itemBufCap);
				}
				idx = itemBufLen;
				++itemBufLen;
			}

			udBuf[idx] = std::forward<UD>(ud);
			auto& o = itemBuf[idx];
			//o.x = x;
			//o.y = y;

			int* c;
			int rowIdx = x < 0 ? -1 : (int)(x / cellWidth);
			int colIdx = y < 0 ? -1 : (int)(y / cellWidth);
			if (rowIdx >= 0 && colIdx >= 0 && rowIdx < rowCount && colIdx < columnCount) {
				o.cellIdx = rowIdx * columnCount + colIdx;
				c = &cells[o.cellIdx];
			}
			else {
				o.cellIdx = -1;
				c = &outside;
			}

			o.prev = -1;
			o.next = *c;
			if (*c != -1) {
				assert(itemBuf[*c].prev == -1);
				itemBuf[*c].prev = idx;
			}
			*c = idx;
			assert(*c == -1 || itemBuf[*c].prev == -1);

			return idx;
		}

		bool ItemUpdate(int const& idx, float const& x, float const& y) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(udBuf[idx].ud);

			auto& o = itemBuf[idx];
			//if (o.x == x && o.y == y) return o.cellIdx != -1;
			//o.x = x;
			//o.y = y;

			// 计算出格子下标
			int rowIdx = x < 0 ? -1 : (int)(x / cellWidth);
			int colIdx = y < 0 ? -1 : (int)(y / cellWidth);
			int cellIdx;
			if (rowIdx >= 0 && colIdx >= 0 && rowIdx < rowCount && colIdx < columnCount) {
				cellIdx = rowIdx * columnCount + colIdx;
			}
			else {
				cellIdx = -1;
			}

			// 无变化就退出
			if (o.cellIdx == cellIdx) return cellIdx != -1;

			// 定位到 item 旧链入口
			int* c;
			if (o.cellIdx != -1) {
				c = &cells[o.cellIdx];
			}
			else {
				c = &outside;
			}
			assert(*c == -1 || itemBuf[*c].prev == -1);

			// 从旧链移除
			if (o.prev != -1) {
				itemBuf[o.prev].next = o.next;
			}
			if (o.next != -1) {
				itemBuf[o.next].prev = o.prev;
			}
			if (*c == idx) {
				*c = o.next;
			}
			assert(*c == -1 || itemBuf[*c].prev == -1);

			// 定位到 item 新入口
			if (cellIdx != -1) {
				c = &cells[cellIdx];
			}
			else {
				c = &outside;
			}

			// 放入新链
			o.prev = -1;
			o.next = *c;
			if (*c != -1) {
				assert(itemBuf[*c].prev == -1);
				itemBuf[*c].prev = idx;
			}
			*c = idx;
			o.cellIdx = cellIdx;
			return cellIdx != -1;
		}

		void ItemRemoveAt(int const& idx) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(itemBuf[idx].ud);
			auto& o = itemBuf[idx];

			// 定位到 item 链入口
			int* c;
			if (o.cellIdx != -1) {
				c = &cells[o.cellIdx];
			}
			else {
				c = &outside;
			}

			// 从链表移除
			if (o.prev != -1) {
				itemBuf[o.prev].next = o.next;
			}
			if (o.next != -1) {
				itemBuf[o.next].prev = o.prev;
			}
			if (*c == idx) {
				*c = o.next;
			}

			// 重置用户数据
			if constexpr (!std::is_standard_layout_v<UD>) {
				udBuf[idx] = UD();
			}

			// 放入 free 单链
			o.next = freeItemHeader;
			freeItemHeader = idx;
			++freeItemCount;
		}

		UD& UDAt(int const& idx) const {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			return udBuf[idx];
		}

		// ForeachItemIndex([](int const& idx){  ctx.UDAt(idx)  }); 
		template<typename F>
		void ForeachItemIndex(F&& f) {
			int cellCount = rowCount * columnCount;
			for (int i = 0; i < cellCount; ++i) {
				if (cells[i] != -1) {
					int c = cells[i];
					while (c != -1) {
						f(c);
						c = itemBuf[c].next;
					}
				}
			}
			if (outside != -1) {
				int c = outside;
				while (c != -1) {
					f(c);
					c = itemBuf[c].next;
				}
			}
		}

		int Count() const {
			return itemBufLen - freeItemCount;
		}

		// 检索 item 覆盖到的格子的邻居 items idxs
		int ItemQueryNears(int const& idx, std::vector<int>& out) {
			assert(idx >= 0);
			assert(idx < itemBufLen);
			//assert(udBuf[idx].ud);
			out.clear();
			auto& o = itemBuf[idx];
			if (o.cellIdx == -1) return 0;
			int c = cells[o.cellIdx];
			while (c != -1) {
				if (c != idx) {
					out.push_back(c);
				}
				c = itemBuf[c].next;
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

			ForeachItemIndex([this](int const& idx) {
				auto& o = itemBuf[idx];
				if (o.cellIdx == -1) return;
				int rIdx = o.cellIdx / columnCount;
				int cIdx = o.cellIdx - rIdx * columnCount;
				std::cout << "idx = " << idx /*<< ", x = " << o.x << ", y = " << o.y*/
					<< ", rIdx = " << rIdx << ", cIdx = " << cIdx << std::endl;
			});

			std::cout << std::endl;
		};
	};
}
