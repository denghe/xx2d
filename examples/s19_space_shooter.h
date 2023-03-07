#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace SpaceShooter {

	struct Scene;

	//...

	struct Monster {
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max() };
		xx::Shared<xx::MovePathCache> mpc;
		xx::XY originalPos{}, pos{};
		float radians{}, speed{}, movedDistance{};
		void Init(Scene* owner_, xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_, float const& speed_, xx::RGBA8 const& color_);
		int Update();
		void Draw();
		xx::Quad body;
		xx::RGBA8 color;
	};

	struct Bullet {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float speed{}, life{};
		void Init(Scene* owner, xx::XY const& pos);
		int Update();
		void Draw();
	};

	struct Plane {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float speed{}, frame{};
		int fireCD{};
		void Init(Scene* owner);
		int Update();
		void Draw();
	};

	struct Space {
		Scene* owner{};
		xx::Quad body;
		float yPos{}, yInc{}, ySize{};
		void Init(Scene* owner);
		int Update();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Coro SceneLogic();
		xx::Shared<Monster>& AddMonster();
		void EraseMonster(Monster* m);

		// res
		xx::TP tp;
		std::vector<xx::Shared<xx::Frame>> framesPlane;
		std::vector<xx::Shared<xx::Frame>> framesNumber;
		std::vector<xx::Shared<xx::Frame>> framesBullet;
		std::vector<xx::Shared<xx::Frame>> framesRocket;
		std::vector<xx::Shared<xx::Frame>> framesMonster;
		std::vector<xx::Shared<xx::Frame>> framesBackground;

		std::vector<xx::Shared<xx::MovePathCache>> mpcsMonster;
		// ... bullet? item?

		// env
		float timePool{};
		float bgScale{}, scale{};
		int frameCounter{};

		Space space;
		Plane plane;
		std::vector<xx::Shared<Bullet>> bullets;
		std::vector<xx::Shared<Monster>> monsters;
		// ...

		xx::Coros coros;
	};
}
