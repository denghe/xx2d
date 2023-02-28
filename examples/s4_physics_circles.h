#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"
#include <xx_threadpool.h>

namespace PhysicsCircles {

#define ENABLE_MULTITHREAD_UPDATE
#define THREAD_POOL_USE_RUN_ONCE
#define NUM_UPDATE_THREADS 16

	struct Circle : xx::SpaceGridCItem<Circle> {
		xx::Pos<> newPos;	// calc result
		int32_t radius{};

		int32_t csIndex{ -1 };	// fill when push to cs, update when swap remove
		std::unique_ptr<xx::LineStrip> border;

		void Init(xx::SpaceGridC<Circle>* const& grid_, int32_t const& x, int32_t const& y, int32_t const& r);	// grid.Add
		void Update(xx::Rnd& rnd);	// calc ( parallelable )
		void Update2();	// grid.Update
		~Circle();	// grid.Remove
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int32_t Update() override;

		inline static constexpr int32_t numRows = 400, numCols = 400, minRadius = 16, maxRadius = 32;
		inline static constexpr int32_t maxDiameter = maxRadius * 2;
		inline static constexpr int32_t maxX = maxDiameter * numCols, maxY = maxDiameter * numRows;
		inline static constexpr int32_t speed = 5, speedMaxScale = 5;
		inline static constexpr int32_t numCircleSegments = 8;
		inline static constexpr int32_t foreachLimit = 12;
		inline static constexpr int32_t numRandCircles = 100000, capacity = numRandCircles * 2;
		inline static constexpr int32_t numEveryInsert = 1000;
		xx::SpaceGridC<Circle> grid;
		xx::SpaceGridCCamera<Circle> cam;
		std::vector<xx::Shared<Circle>> cs;
		double timePool{};
		std::vector<Circle*> tmpcs;

		std::array<xx::Rnd, NUM_UPDATE_THREADS> rnds;

#ifdef ENABLE_MULTITHREAD_UPDATE
		xx::ToggleThreadPool ttp;
#endif
	};

}
