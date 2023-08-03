#pragma once
#include "main.h"

namespace ShootGame {

	static constexpr const float M_SQ = 0.7071067811865475244;

	struct Player;
	struct Bullet;
	struct Monster;
	struct HPLabel;

	struct Scene {
		GameLooper* looper;
		xx::Shared<xx::GLTexture> tex;
		xx::Shared<Player> player;
		std::vector<xx::Shared<Monster>> monsters;
		std::vector<xx::Shared<HPLabel>> labels;
		std::vector<xx::Sprite*> tmp;
		xx::Tasks tasks;
		xx::Rnd rnd;
		bool running{};

		void Init(GameLooper* looper);
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

		void Init(Scene* const& scene, Player* const& owner, xx::XY const& inc, int const& life);
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
		int life{};

		void Init(Scene* const& scene);
		int Update();

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
