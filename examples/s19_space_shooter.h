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
		int64_t avaliableFrameNumber{};
		xx::Label body;

		void Init(Scene* const& owner_, xx::XY const& pos_, std::string_view const& txt_);
		bool Update();
		void Draw();
	};


	template<typename T>
	using Listener = std::function<void(T*)>;
	template<typename T>
	using Listener_s = xx::Shared<Listener<T>>;

	struct MonsterBase {
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max() };
		xx::XY pos{};
		float radius{}, radians{}, speed{};
		int64_t hp{}, maxHP{}, hitEffectExpireFrameNumber{};
		Listener_s<MonsterBase> deathListener;
		xx::Quad body;
		xx::RGBA8 color;
		void Draw();
		bool Hit(int64_t const& damage);	// return true: dead
		virtual bool Update() = 0;
	};
	struct Monster : MonsterBase {
		xx::Shared<xx::MovePathCache> mpc;
		xx::XY originalPos{};
		float movedDistance{};

		void Init(Scene* owner_, xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_, float const& speed_ = 1.f, xx::RGBA8 const& color_ = {255,255,255,255}, Listener_s<MonsterBase> deathListener_ = {});
		bool Update() override;
	};
	struct Monster2 : MonsterBase {
		xx::XY inc{};
		int64_t avaliableFrameNumber{};

		void Init(Scene* owner_, xx::XY const& posFrom, float const& radians_, float const& speed_ = 1.f, xx::RGBA8 const& color_ = {255,255,255,255}, Listener_s<MonsterBase> deathListener_ = {});
		bool Update() override;
	};

	struct Bullet {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float radius{}, speed{};
		int64_t damage{}, avaliableFrameNumber{};
		void Init(Scene* owner_, xx::XY const& pos_, int64_t const& power_);
		bool Update();
		void Draw();
	};

	struct Plane {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float speed{}, frame{};
		int64_t fireCD{};
		void Init(Scene* owner);
		bool Update();
		void Draw();
	};

	struct Space {
		Scene* owner{};
		xx::Quad body;
		float yPos{}, yInc{}, ySize{};
		void Init(Scene* owner);
		void Update();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Coro SceneLogic();
		xx::Coro SceneLogic_CreateMonsterTeam(int n, int64_t bonus);

		void AddMonster(MonsterBase* m);	// insert into monsters & sync index
		void EraseMonster(MonsterBase* m);	// remove from monsters & clear index

		void ShowBonusEffect(xx::XY const& pos, int64_t const& value);

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
		int64_t frameNumber{};
		xx::Rnd rnd;

		Space space;
		Score score;
		Plane plane;
		std::vector<xx::Shared<Bullet>> bullets;
		std::vector<xx::Shared<MonsterBase>> monsters;
		std::vector<xx::Shared<LabelEffect>> labels;
		// ...

		xx::Coros coros;
	};
}
