#pragma once
#include "main.h"

namespace ShootGameWithIndex {

	static constexpr const float M_SQ = 0.7071067811865475244;

	struct Player;
	struct Bullet;
	struct Monster;
	struct HPLabel;
	using MonsterIter = xx::ListDoubleLink<Monster>::IndexAndVersion;

	struct Scene {
		GameLooper* looper{};
		xx::SpaceGridC<Monster> sgMonsters;	// ensure life cycle

		xx::Shared<xx::GLTexture> tex;
		xx::Shared<Player> player;
		xx::ListDoubleLink<Monster> monsters;
		xx::ListLink<HPLabel> labels;
		std::vector<xx::Quad*> tmp;
		xx::Coros coros;
		xx::Rnd rnd;
		bool running{};

		void EraseMonster(Monster* const& m);

		void Init(GameLooper* looper);
		void Update();
		void Draw();
	};

	struct Player {
		Scene* scene{};

		xx::Quad body;
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

		xx::Quad body;
		xx::LineStrip ring;
		xx::XY pos{}, inc{};
		float radius{}, speed{};
		int life{};

		void Init(Scene* const& scene, Player* const& owner, xx::XY const& inc, int const& life);
		int Update();

		void DrawInit();
		void DrawCommit();
	};

	struct Monster : xx::SpaceGridCItem<Monster> {
		Scene* scene{};

		xx::Quad body;
		xx::LineStrip ring;
		xx::XY pos{}, inc{};
		float radius{}, speed{};
		int life{};
		MonsterIter indexAtMonsters{};

		void Init(Scene* const& scene, MonsterIter const& indexAtMonsters);
		int Update();
		~Monster();

		void DrawInit();
		void DrawCommit();
	};

	struct HPLabel {
		Scene* scene{};
		xx::SimpleLabel lbl;
		std::string txt;
		xx::XY pos{}, inc{};
		int life{};

		void Init(Scene* const& scene, xx::XY const& pos, int64_t const& hp);
		int Update();

		void DrawInit();
		void DrawCommit();
	};

	struct Looper : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		Scene scene;
		double timePool{};
	};

}
