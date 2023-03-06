#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace SpaceShooter {

	struct Scene;

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
		float speed{};
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

		// res manage
		xx::TP tp;
		std::vector<xx::Shared<xx::Frame>> framesPlane;
		std::vector<xx::Shared<xx::Frame>> framesNumber;
		std::vector<xx::Shared<xx::Frame>> framesBullet;
		std::vector<xx::Shared<xx::Frame>> framesRocket;
		std::vector<xx::Shared<xx::Frame>> framesMonster;
		std::vector<xx::Shared<xx::Frame>> framesBackground;

		// env
		float timePool{};
		float bgScale{}, scale{};

		Space space;
		Plane plane;
		std::vector<xx::Shared<Bullet>> bullets;
		// ...
	};
}
