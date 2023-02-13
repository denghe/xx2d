#pragma once
#include "pch.h"
#include "logic_base.h"

namespace SG {

	static constexpr const float M_SQ = 0.7071067811865475244;

	struct Player;
	struct Bullet;
	struct Monster;

	struct Scene {
		xx::Shared<xx::GLTexture> tex;
		xx::Shared<Player> player;
		std::vector<xx::Shared<Monster>> monsters;
		std::vector<xx::Sprite*> tmp;

		void Init();
		void Update();
		void Draw();
	};

	struct Player {
		Scene* scene{};

		xx::Sprite body;
		xx::LineStrip ring;	// for debug
		xx::XY pos{};
		float radius{ 32 }, speed{ 2 };

		float bulletRadius{ 8 }, bulletSpeed{ 4 };
		std::vector<xx::Shared<Bullet>> bullets;

		void Init(Scene* scene);
		void Update();

		void DrawInit();
		void DrawCommit();
	};

	struct Bullet {
		Scene* scene{};
		Player* owner{};

		xx::Sprite body;
		xx::LineStrip ring;
		xx::XY pos{}, inc{};
		float radius{}, speed{};
		int life{};

		void Init(Scene* scene, Player* owner, xx::XY const& inc, int const& life);
		int Update();

		void DrawInit();
		void DrawCommit();
	};

	struct Monster {
		Scene* scene{};

		xx::Sprite body;
		xx::LineStrip ring;
		xx::XY pos{}, inc{};
		float radius{}, speed{};

		void Init(Scene* scene);
		void Update();

		void DrawInit();
		void DrawCommit();
	};
}

struct Logic11 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	SG::Scene scene;
	double timePool{};
};
