#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace SpaceShooter {

	struct Scene;

	//...

	struct Score {
		Scene* owner{};
		int64_t from{}, to{}, step{};

		void Init(Scene* const& owner_);
		void Add(int64_t const& offset);
		void Update();
		void Draw();
	};

	struct LabelEffect {
		Scene* owner{};
		xx::XY pos{}, inc{};
		int life{};
		xx::Label body;

		void Init(Scene* const& owner_, xx::XY const& pos_, std::string_view const& txt_);
		int Update();
		void Draw();
	};


	template<typename T>
	using Listener = std::function<void(T*)>;
	template<typename T>
	using Listener_s = xx::Shared<Listener<T>>;


	struct Monster {
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max() };

		xx::Shared<xx::MovePathCache> mpc;
		xx::XY originalPos{}, pos{};
		float radius{}, radians{}, speed{}, movedDistance{};
		int64_t hp{}, maxHP{}, hitEffectEndFrame{};
		Listener_s<Monster> deathListener;

		void Init(Scene* owner_, xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_, float const& speed_ = 1.f, xx::RGBA8 const& color_ = {255,255,255,255}, Listener_s<Monster> deathListener_ = {});
		int Update();
		bool Hit(int64_t const& damage);	// return true: dead
		void Draw();

		xx::Quad body;
		xx::RGBA8 color;
	};

	struct Bullet {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float radius{}, speed{}, life{};
		int64_t damage{};
		void Init(Scene* owner_, xx::XY const& pos_, int64_t const& power_);
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
		// ... mpcsBullet? mpcsStuff?

		// env
		float timePool{};
		float bgScale{}, scale{};
		int frameCounter{};

		Space space;
		Score score;
		Plane plane;
		std::vector<xx::Shared<Bullet>> bullets;
		std::vector<xx::Shared<Monster>> monsters;
		std::vector<xx::Shared<LabelEffect>> labels;
		// ...

		xx::Coros coros;
	};
}
