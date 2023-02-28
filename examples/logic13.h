#pragma once
#include "xx2d_pch.h"
#include "logic_base.h"
#include <xx_coro_simple.h>

namespace MovePathTests {

	struct Scene;
	struct Monster {
		// fill by CreateMonster & ReleaseMonster
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max()};

		xx::Shared<xx::MovePathCache> mpc;
		xx::XY originalPos{}, pos{};
		float radians{}, speed{ 1 }, movedDistance{};
		//xx::LineStrip body;
		xx::Quad body;

		void Init(xx::XY const& pos, xx::Shared<xx::MovePathCache> mpc, float const& speed, xx::RGBA8 const& color);
		int Update();	// return non zero: release
		void DrawInit(xx::RGBA8 const& color);
		void Draw();
	};

	struct Scene {
		GameLooper* looper{};
		xx::Shared<xx::GLTexture> tex;
		xx::Rnd rnd;
		std::vector<xx::Shared<Monster>> monsters;
		xx::Coros coros;

		xx::Shared<Monster>& AddMonster();
		void EraseMonster(Monster* m);

		void Init(GameLooper* looper);
		void Update();
		void Draw();
	};

}

struct Logic13 : LogicBase {
	void Init(GameLooper* looper) override;
	int Update() override;

	MovePathTests::Scene scene;
	double timePool{};
};
