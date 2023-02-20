#pragma once
#include "pch.h"
#include "logic_base.h"
#include <xx_coro_simple.h>

namespace MovePathTests {

	struct MovePathPoint {
		xx::XY pos{}, inc{};
		float radians{}, distance{};
	};

	struct MovePath {
		std::vector<MovePathPoint> points;
		float totalDistance{};
		bool loop{};
		void Fill(xx::XY const* ps, size_t len, bool loop);
		void Fill(bool loop);
		void FillFields(MovePathPoint& p1, MovePathPoint& p2);
	};

	struct MovePathRunner {
		xx::Shared<MovePath> mp;
		size_t cursor{};	// mp[ index ]
		float cursorDistance{};	// forward
		void Init(xx::Shared<MovePath> mp);

		struct MoveResult {
			xx::XY pos{};
			float radians{}, movedDistance{};
			bool terminated;
		};
		MoveResult MoveToBegin();
		MoveResult MoveToEnd();
		MoveResult MoveForward(float const& distance);
		MoveResult MoveBackward(float const& distance);
	};

	struct Scene;
	struct Monster {
		// fill by CreateMonster & ReleaseMonster
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max()};

		MovePathRunner mpr;
		xx::XY pos{}, fixedPos{};
		float radians{}, speed{ 1 };
		xx::LineStrip body;

		void Init(xx::XY const& pos, xx::Shared<MovePath> mp);
		int Update();	// return non zero: release
		void DrawInit();
		void Draw();
	};

	struct Scene {
		Logic* logic{};
		std::vector<xx::Shared<Monster>> monsters;
		xx::Coros coros;

		xx::Shared<Monster>& AddMonster();
		void EraseMonster(Monster* m);

		void Init(Logic* logic);
		void Update();
		void Draw();
	};

}

struct Logic13 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	MovePathTests::Scene scene;
	double timePool{};
};
