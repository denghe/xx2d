#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>
#include <algorithm>
#include "xx_linkpool.h"

// todo: 似乎还有 bug

namespace xx {

	// 格子空间 点坐标 索引系统
	// 针对 Cell 尺寸大于所有 Item 的情况, 将空间索引系统简化为 中心点 数据管理. Item 和 Cell 1对1, Cell 和 Item 1对多 不重复
	// Cell 即 Item 链表头 ( 类似 hash map 的 bucket )
	// UD 和 Item 分离存储，下标一致, 确保内存集中分布提升查询效率
	// 如果真出现 Item 大于 Cell 尺寸的情况, 可以创建多套 Grid, 分级管理( 类似4叉树的感觉 ), 合并查询结果

	namespace Space2dPointIndex {

		// todo: 支持设置 original 0,0 点所在格子 以方便使用

		template<typename UD, typename XYType = float>
		struct Grid {
			struct Item {
				XYType x, y;
				int prev, next;
				int cellIdx;
			};

			XYType cellWH;
			int rowCount;
			int columnCount;
			int radius;

		protected:
			int* cells;
			int outside;

			LinkPool<Item> items;
			std::vector<UD> uds;

			// for sort by r
			struct IR {
				int i;
				XYType r;
			};
			struct IRComparer {
				bool operator()(IR const& a, IR const& b) const { return a.r < b.r; }
			} cmp;
			std::vector<IR> irs;

		public:
			Grid(XYType const& cellWH, int const& rowCount, int const& columnCount, int const& cap)
				: cellWH(cellWH)
				, rowCount(rowCount)
				, columnCount(columnCount)
				, items(cap)
			{
				radius = std::max(rowCount, columnCount) / 2 + 1;

				cells = (int*)malloc(sizeof(int) * rowCount * columnCount);

				uds.resize(cap);

				Clear<true>();
			}

			template<bool isCtor = false>
			void Clear() {
				if constexpr (!isCtor && !std::is_standard_layout_v<UD>) {
					ForeachItemIndex([this](int const& idx) {
						uds[idx] = UD();
						});
				}
				memset((void*)cells, -1, sizeof(int) * rowCount * columnCount);
				outside = -1;

				items.Clear();
			}

			~Grid() {
				free(cells);
			}

			int ItemAdd(UD&& ud, XYType const& x, XYType const& y) {
				//assert(ud);
				int idx;
				if (auto&& cap = items.Alloc(idx)) {
					uds.resize(cap);
				}

				uds[idx] = std::forward<UD>(ud);
				auto& o = items[idx];
				o.x = x;
				o.y = y;

				int* c;
				int rIdx = x < 0 ? -1 : (int)(x / cellWH);
				int cIdx = y < 0 ? -1 : (int)(y / cellWH);
				if (rIdx >= 0 && cIdx >= 0 && rIdx < rowCount && cIdx < columnCount) {
					o.cellIdx = rIdx * columnCount + cIdx;
					c = &cells[o.cellIdx];
				}
				else {
					o.cellIdx = -1;
					c = &outside;
				}

				o.prev = -1;
				o.next = *c;
				if (*c != -1) {
					assert(items[*c].prev == -1);
					items[*c].prev = idx;
				}
				*c = idx;
				assert(*c == -1 || items[*c].prev == -1);

				return idx;
			}

			bool ItemUpdate(int const& idx, XYType const& x, XYType const& y) {
				assert(idx >= 0);
				assert(idx < items.Len());
				//assert(uds[idx].ud);

				auto& o = items[idx];
				//if (o.x == x && o.y == y) return o.cellIdx != -1;
				o.x = x;
				o.y = y;

				// 计算出格子下标
				int rIdx = x < 0 ? -1 : (int)(x / cellWH);
				int cIdx = y < 0 ? -1 : (int)(y / cellWH);
				int cellIdx;
				if (rIdx >= 0 && cIdx >= 0 && rIdx < rowCount && cIdx < columnCount) {
					cellIdx = rIdx * columnCount + cIdx;
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
				assert(*c == -1 || items[*c].prev == -1);

				// 从旧链移除
				if (o.prev != -1) {
					items[o.prev].next = o.next;
				}
				if (o.next != -1) {
					items[o.next].prev = o.prev;
				}
				if (*c == idx) {
					*c = o.next;
				}
				assert(*c == -1 || items[*c].prev == -1);

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
					assert(items[*c].prev == -1);
					items[*c].prev = idx;
				}
				*c = idx;
				o.cellIdx = cellIdx;
				return cellIdx != -1;
			}

			void ItemRemoveAt(int const& idx) {
				assert(idx >= 0);
				assert(idx < items.Len());
				//assert(items[idx].ud);
				auto& o = items[idx];

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
					items[o.prev].next = o.next;
				}
				if (o.next != -1) {
					items[o.next].prev = o.prev;
				}
				if (*c == idx) {
					*c = o.next;
				}

				// 重置用户数据
				if constexpr (!std::is_standard_layout_v<UD>) {
					uds[idx] = UD();
				}

				// 释放
				items.Free(idx);
			}

			UD const& UDAt(int const& idx) const {
				assert(idx >= 0);
				assert(idx < items.Len());
				return uds[idx];
			}
			UD& UDAt(int const& idx) {
				assert(idx >= 0);
				assert(idx < items.Len());
				return uds[idx];
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
							c = items[c].next;
						}
					}
				}
				if (outside != -1) {
					int c = outside;
					while (c != -1) {
						f(c);
						c = items[c].next;
					}
				}
			}

			int Count() const {
				//return itemBufLen - freeItemCount;
				return items.Count();
			}

			// 检索 x,y 所在格子 + 周围 8 格 的 items idxs
			int ItemQueryNeighbor(std::vector<int>& out, XYType const& x, XYType const& y) {
				out.clear();
				// 计算出 x,y 所在 格子下标
				int rIdx = y < 0 ? -1 : (int)(y / cellWH);
				int cIdx = x < 0 ? -1 : (int)(x / cellWH);
				if (!(rIdx >= 0 && cIdx >= 0 && rIdx < rowCount && cIdx < columnCount)) return 0;

				PushTo(rIdx, cIdx, out);
				if (cIdx)
					PushTo(rIdx, cIdx - 1, out);
				if (cIdx + 1 < columnCount)
					PushTo(rIdx, cIdx + 1, out);

				if (rIdx) {
					PushTo(rIdx - 1, cIdx, out);
					if (cIdx)
						PushTo(rIdx - 1, cIdx - 1, out);
					if (cIdx + 1 < columnCount)
						PushTo(rIdx - 1, cIdx + 1, out);
				}

				if (rIdx + 1 < rowCount) {
					PushTo(rIdx + 1, cIdx, out);
					if (cIdx)
						PushTo(rIdx + 1, cIdx - 1, out);
					if (cIdx + 1 < columnCount)
						PushTo(rIdx + 1, cIdx + 1, out);
				}

				return (int)out.size();
			}

			// 检索 item 半径 r 的圆覆盖到的格子的邻居 items idxs( 只将周围 8 格包含在内, r 不得大于 cell size )
			int ItemQueryNeighborCircle(std::vector<int>& out, XYType const& x, XYType const& y, XYType const& r) {
				assert(r > 0 && r <= cellWH);

				out.clear();
				// 计算出 x,y 所在 格子下标
				int rIdx = y < 0 ? -1 : (int)(y / cellWH);
				int cIdx = x < 0 ? -1 : (int)(x / cellWH);
				if (!(rIdx >= 0 && cIdx >= 0 && rIdx < rowCount && cIdx < columnCount)) return 0;

				PushTo(rIdx, cIdx, out, x, y, r);
				if (cIdx)
					PushTo(rIdx, cIdx - 1, out, x, y, r);
				if (cIdx + 1 < columnCount)
					PushTo(rIdx, cIdx + 1, out, x, y, r);

				if (rIdx) {
					PushTo(rIdx - 1, cIdx, out, x, y, r);
					if (cIdx)
						PushTo(rIdx - 1, cIdx - 1, out, x, y, r);
					if (cIdx + 1 < columnCount)
						PushTo(rIdx - 1, cIdx + 1, out, x, y, r);
				}

				if (rIdx + 1 < rowCount) {
					PushTo(rIdx + 1, cIdx, out, x, y, r);
					if (cIdx)
						PushTo(rIdx + 1, cIdx - 1, out, x, y, r);
					if (cIdx + 1 < columnCount)
						PushTo(rIdx + 1, cIdx + 1, out, x, y, r);
				}

				return (int)out.size();
			}

			// 从 x,y 当前所在格子开始，向外圆形 扩散, 将 idx 塞入 irs, 塞够 limit 个停止扩散, 返回实际塞入个数
			int ItemQueryNums(int const& limit, XYType const& x, XYType const& y, XYType const& r) {
				int rIdx = y < 0 ? -1 : (int)(y / cellWH);
				int cIdx = x < 0 ? -1 : (int)(x / cellWH);
				if (!(rIdx >= 0 && cIdx >= 0 && rIdx < rowCount && cIdx < columnCount)) return 0;

				irs.clear();

				// 格坐标 判定用半径pow
				XYType rr = (r + cellWH) * (r + cellWH);

				// 因为对称，故可以在 for 的时候先根据距离，计算看要跳过多少个格子， 两头减去
#define RR_RI_CI_CHECK_RANGE(ri, ci) ((rIdx - ri) * cellWH) * ((rIdx - ri) * cellWH) + ((cIdx - ci) * cellWH) * ((cIdx - ci) * cellWH) < rr

			// 当前格
				PushTo(rIdx, cIdx, irs);

				// 开始扩散. 先上下横边 再 左右竖边
				int jb, je, kb, ke;
				for (int i = 1; i <= radius; i++) {

					kb = std::max(cIdx - i, 0);
					ke = std::min(cIdx + i, columnCount - 1);

					// 上横
					jb = rIdx - i;
					if (jb >= 0) {
						// 探索位于圆形范围内的格子坐标边界
						int k = kb;
						for (; k <= ke; k++) {
							if (RR_RI_CI_CHECK_RANGE(jb, k)) {
								ke -= kb - k;
								break;
							}
						}
						for (; k <= ke; k++) {
							PushTo(jb, k, irs);
						}
					}

					// 下横
					je = rIdx + i;
					if (je < rowCount) {
						int k = kb;
						for (; k <= ke; k++) {
							if (RR_RI_CI_CHECK_RANGE(jb, k)) {
								ke -= kb - k;
								break;
							}
						}
						for (; k <= ke; k++) {
							PushTo(je, k, irs);
						}
					}

					jb = std::max(jb + 1, 0);
					je = std::min(je - 1, rowCount - 1);

					// 左边
					kb = cIdx - i;
					if (kb >= 0) {
						int j = jb;
						for (; j <= je; j++) {
							if (RR_RI_CI_CHECK_RANGE(j, kb)) {
								je -= jb - j;
								break;
							}
						}
						for (; j <= je; j++) {
							PushTo(j, kb, irs);
						}
					}

					// 右边
					ke = cIdx + i;
					if (ke < columnCount) {
						int j = jb;
						for (; j <= je; j++) {
							if (RR_RI_CI_CHECK_RANGE(j, kb)) {
								je -= jb - j;
								break;
							}
						}
						for (; j <= je; j++) {
							PushTo(j, ke, irs);
						}
					}

					// 数量满足，退出扩散
					if (irs.size() >= limit) break;
				}

#undef RR_RI_CI_CHECK_RANGE

				return (int)irs.size();
			}

			// cell 向外圆形 扩散. 扩一圈就将区域内的 items 放入 irs. 直到数量足够, 最后根据距离排序，留下 limit 个
			int ItemQueryNears(std::vector<int>& out, int limit, XYType const& x, XYType const& y, XYType const& r = XYType()) {
				assert(limit > 0);
				assert(r >= 0);

				out.clear();
				if (!ItemQueryNums(limit, x, y, r)) return 0;

				// 算距离 顺便过滤下
				XYType rr = r * r;
				for (int i = (int)irs.size() - 1; i >= 0; --i) {
					auto& ir = irs[i];
					auto& o = items[ir.i];
					ir.r = (o.x - x) * (o.x - x) + (o.y - y) * (o.y - y);
					if (rr > 0 && ir.r > rr) {
						irs[i] = irs.back();
						irs.pop_back();
					}
				}

				if (limit == 1) {
					if (int siz = (int)irs.size()) {
						int n = 0;
						for (int i = 1; i < siz; ++i) {
							if (irs[n].r > irs[i].r) {
								n = i;
							}
						}
						out.push_back(irs[n].i);
					}
				}
				else {
					// 按距离排序( 从小到大 )
					std::sort(irs.begin(), irs.end(), cmp);

					limit = std::min(limit, (int)irs.size());
					for (int i = 0; i < limit; ++i) {
						out.push_back(irs[i].i);
					}
				}
				return (int)out.size();
			}

			void Dump() {
				int c = Count();
				std::cout << "---------------- item's count = " << c << std::endl;
				if (!c) return;

				ForeachItemIndex([this](int const& idx) {
					auto& o = items[idx];
					if (o.cellIdx == -1) return;
					int rIdx = o.cellIdx / columnCount;
					int cIdx = o.cellIdx - rIdx * columnCount;
					std::cout << "idx = " << idx /*<< ", x = " << o.x << ", y = " << o.y*/
						<< ", rIdx = " << rIdx << ", cIdx = " << cIdx << std::endl;
					});

				std::cout << std::endl;
			};

		protected:

#ifndef XX_FORCEINLINE
#ifndef NDEBUG
#define XX_FORCEINLINE
#else
#ifdef _MSC_VER
#define XX_FORCEINLINE __forceinline
#else
#define XX_FORCEINLINE __attribute__((always_inline))
#endif
#endif
#endif

			// 将一个 cell 下所有 items 压入 out
			template<typename VT>
			XX_FORCEINLINE void PushTo(int const& rIdx, int const& cIdx, VT& out) {
				int c = cells[rIdx * columnCount + cIdx];
				while (c != -1) {
					out.emplace_back(c);
					c = items[c].next;
				}
			}

			// 将一个 cell 下所有 在 x,y,r 圆范围内的 items 压入 out
			template<typename VT>
			XX_FORCEINLINE void PushTo(int const& rIdx, int const& cIdx, VT& out, XYType const& x, XYType const& y, XYType const& r) {
				int c = cells[rIdx * columnCount + cIdx];
				XYType rr = r * r;
				while (c != -1) {
					auto& o = items[c];
					if ((x - o.x) * (x - o.x) + (y - o.y) * (y - o.y) <= rr) {
						out.emplace_back(c);
					}
					c = o.next;
				}
			}

		};
	}

}
