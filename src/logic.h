#pragma once
#include "pch.h"

namespace GameLogic {
	struct Scene;
}

struct Logic : Engine {
	//Rnd rnd;
	double timePool{};
	//xx::Coros coros;

	//std::set<xx::Shared<Monster>> monsters;
	std::vector<Sprite*> tmp;

	xx::Shared<GameLogic::Scene> scene;

	int64_t objsCounter{};
	xx::Shared<Frame> fZazaka, fMouse;
	BMFont fnt1, fnt2;
	Label lbCount;

	void Init();
	int Update();
};

namespace GameLogic {
	// 下列代码模拟 一组 圆形对象, 位于 2d 空间，一开始挤在一起，之后物理随机排开, 直到静止。统计一共要花多长时间。
	// 数据结构方面不做特殊优化，走正常 oo 风格
	// 要对比的是 用 或 不用 grid 系统的效率

	// 坐标类型限定
	using XY = xx::XY<int>;

	// 模拟配置. 格子尺寸, grid 列数均为 2^n 方便位运算
	static const int gridWidth = 512;
	static const int gridHeight = 512;
	static const int gridDiameter = 64;

	static const int mapMinX = gridDiameter;							// >=
	static const int mapMaxX = gridDiameter * (gridWidth - 1) - 1;		// <=
	static const int mapMinY = gridDiameter;
	static const int mapMaxY = gridDiameter * (gridHeight - 1) - 1;
	static const XY mapCenter = { (mapMinX + mapMaxX) / 2, (mapMinY + mapMaxY) / 2 };

	// Foo 的半径 / 直径 / 移动速度( 每帧移动单位距离 ) / 邻居查找个数
	static const int fooRadius = 50;
	static const int fooSpeed = 10;
	static const int fooFindNeighborLimit = 20;	// 9格范围内通常能容纳的个数

	struct Scene;
	struct Foo {
		// 当前半径
		int16_t radius = fooRadius;
		// 当前速度
		int16_t speed = fooSpeed;
		// 所在 grid 下标. 放入 im->grid 后，该值不为 -1
		int gridIndex = -1;
		// 当前坐标( Update2 单线程填充, 同时同步 grid )
		XY xy;
		// 即将生效的坐标( Update 多线并行填充 )
		XY xy2;

		Sprite spr;

		// 构造时就放入 grid 系统
		Foo(Scene* const& scene, XY const& xy);

		// 计算下一步 xy 的动向, 写入 xy2
		void Update(Scene* const& scene);

		// 令 xy2 生效, 同步 grid
		void Update2(Scene* const& scene);
	};

	struct Scene {
		Scene() = default;
		Scene(Scene const&) = delete;
		Scene& operator=(Scene const&) = delete;

		// 格子系统( int16 则总 item 个数不能超过 64k, int 就敞开用了 )
		xx::Grid2d<Foo*, int> grid;

		// 对象容器 需要在 格子系统 的下方，确保 先 析构，这样才能正确的从 格子系统 移除
		std::set<xx::Shared<Foo>> objs;
		size_t objsCap;

		// 随机数一枚, 用于对象完全重叠的情况下产生一个移动方向
		inline static thread_local Rnd rnd;

		// 每帧统计还有多少个对象正在移动
		int count = 0;

		// point to engine instance
		Logic* eg;

		void Init(Logic* eg, int const& cap) {
			this->eg = eg;
			objsCap = cap;

			// 初始化 2d 空间索引 容器
			grid.Init(gridHeight, gridWidth, cap);

			//// 直接构造出 n 个 Foo
			//for (int i = 0; i < fooCount; i++) {
			//	objs.emplace_back(this, mapCenter);
			//}
		}

		void Insert(::XY xy) {
			if (objs.size() == objsCap) return;
			objs.insert(xx::Make<Foo>( this, XY{
				(int)((xy.x + eg->w / 2) / eg->w * mapMaxX),
				(int)((xy.y + eg->h / 2) / eg->h * mapMaxY),
			}));
		}

		void Update() {
			count = 0;
			auto siz = objs.size();
			// 这里先简化，没有删除行为。否则就要记录到线程安全队列，事后批量删除
#ifdef OPEN_MP_NUM_THREADS
#pragma omp parallel for
#endif
			for (auto& o : objs) {
				o->Update(this);
			}
			for (auto& o : objs) {
				o->Update2(this);
			}
		}
	};

	inline Foo::Foo(Scene* const& scene, XY const& xy) {
		this->xy = xy;
		gridIndex = scene->grid.Add(xy.y / gridDiameter, xy.x / gridDiameter, this);
		spr.SetTexture(scene->eg->fMouse);
		spr.SetAnchor({ 0.5, 0 });
		auto c = scene->rnd.Get(); auto cp = (uint8_t*)&c;
		spr.SetColor({ cp[0], cp[1], cp[2], 255 });
		spr.SetScale({0.5, 0.5});
		spr.SetPositon({
			((float)xy.x - mapCenter.x) / mapCenter.x * (scene->eg->w / 2),
			((float)xy.y - mapCenter.y) / mapCenter.y * (scene->eg->h / 2)
		});
	}

	inline void Foo::Update(Scene* const& scene) {
		// 提取坐标
		auto xy = this->xy;

		// 判断周围是否有别的 Foo 和自己重叠，取前 n 个，根据对方和自己的角度，结合距离，得到 推力矢量，求和 得到自己的前进方向和速度。
		// 最关键的是最终移动方向。速度需要限定最大值和最小值。如果算出来矢量为 0，那就随机角度正常速度移动。
		// 移动后的 xy 如果超出了 地图边缘，就硬调整. 

		// 查找 n 个邻居
		int limit = fooFindNeighborLimit;
		int crossNum = 0;
		XY v;
		scene->grid.LimitFindNeighbor(limit, gridIndex, [&](auto& o) {
			assert(o != this);
			// 类型转换下方便使用
			auto& f = *o;
			// 如果圆心完全重叠，则不产生推力
			if (f.xy == this->xy) {
				++crossNum;
				return;
			}
			// 准备判断是否有重叠. r1* r1 + r2 * r2 > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
			auto r1 = (int)f.radius;
			auto r2 = (int)this->radius;
			auto r12 = r1 * r1 + r2 * r2;
			auto p1 = f.xy;
			auto p2 = this->xy;
			auto p12 = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
			// 有重叠
			if (r12 > p12) {
				// 计算 f 到 this 的角度( 对方产生的推力方向 ). 
				auto a = xx::GetAngle<(gridDiameter * 2 >= 1024)>(p1, p2);
				// 重叠的越多，推力越大?
				auto inc = xx::Rotate(XY{ this->speed * r12 / p12, 0 }, a);
				v += inc;
				++crossNum;
			}
		});

		if (crossNum) {
			// 如果 v == 0 那就随机角度正常速度移动
			if (v.IsZero()) {
				auto a = scene->rnd.Next() % xx::table_num_angles;
				auto inc = xx::Rotate(XY{ speed, 0 }, a);
				xy += inc;
			}
			// 根据 v 移动
			else {
				auto a = xx::GetAngleXY(v.x, v.y);
				auto inc = xx::Rotate(XY{ speed * std::max(crossNum, 5), 0 }, a);
				xy += inc;
			}

			// 边缘限定( 当前逻辑有重叠才移动，故边界检测放在这里 )
			if (xy.x < mapMinX) xy.x = mapMinX;
			else if (xy.x > mapMaxX) xy.x = mapMaxX;
			if (xy.y < mapMinY) xy.y = mapMinY;
			else if (xy.y > mapMaxY) xy.y = mapMaxY;
		}

		// 保存 xy ( update2 的时候应用 )
		xy2 = xy;
	}

	inline void Foo::Update2(Scene* const& scene) {
		// 变更检测与同步
		if (xy2 != xy) {
			++scene->count;
			xy = xy2;
			assert(gridIndex != -1);
			scene->grid.Update(gridIndex, xy2.y / gridDiameter, xy2.x / gridDiameter);
			spr.SetPositon({ 
				((float)xy.x - mapCenter.x) / mapCenter.x * (scene->eg->w / 2),
				((float)xy.y - mapCenter.y) / mapCenter.y * (scene->eg->h / 2)
			});
		}
	}
}

//
//struct Monster {
//	int64_t id;
//	double x, y;			// pos in global map
//	double drawOrder;		// -(y + id / 1000000000)
//	int64_t hp;
//	Sprite sprite;
//	Label label;
//};
