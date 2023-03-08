#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace SpaceShooter {

	struct Scene;

	//...


	struct DeathEffect {
		Scene* owner{};
		xx::XY pos{};
		float frameIndex{};
		xx::Quad body;

		void Init(Scene* const& owner_, xx::XY const& pos_, float const& scale = 1.f);
		bool Update();
		void Draw();
	};


	struct Power {
		Scene* owner{};
		xx::Shared<xx::MovePathCache> mpc;
		xx::XY pos{}, inc{};
		float movedDistance{}, speed{}, radius{};
		int lineNumber{}, typeId{}, i{};
		xx::Quad body;

		void Init(Scene* const& owner_, xx::XY const& pos_, int const& typeId_);
		int Update();
		void Draw();
	};

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

	struct MonsterBase : xx::SpaceGridCItem<MonsterBase> {
		Scene* owner{};
		size_t indexAtOwnerMonsters{ std::numeric_limits<size_t>::max() };
		xx::XY pos{};
		float radius{}, radians{}, speed{}, frameIndex{}, frameStep{};
		int64_t hp{}, maxHP{}, hitEffectExpireFrameNumber{};
		Listener_s<MonsterBase> deathListener;
		std::vector<xx::Shared<xx::Frame>>* frames{};
		xx::Quad body;
		xx::RGBA8 color;
		void Init1(Scene* owner_, float const& speed_ = 1.f, xx::RGBA8 const& color_ = { 255,255,255,255 }, Listener_s<MonsterBase> deathListener_ = {});
		virtual bool Update() = 0;
		void UpdateFrameIndex();
		bool Hit(int64_t const& damage);	// return true: dead
		void Draw();
		virtual ~MonsterBase();
	};

	struct Monster : MonsterBase {
		xx::Shared<xx::MovePathCache> mpc;
		xx::XY originalPos{};
		float movedDistance{};

		void Init2(xx::XY const& pos_, xx::Shared<xx::MovePathCache> mpc_);
		bool Update() override;
	};

	struct Monster2 : MonsterBase {
		xx::XY inc{};
		int64_t avaliableFrameNumber{};

		void Init2(xx::XY const& pos_, float const& radians_);
		bool Update() override;
	};

	struct Bullet {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float radius{}, speed{};
		int64_t damage{};
		void Init(Scene* owner_, xx::XY const& pos_, int64_t const& power_);
		bool Update();
		void Draw();
	};

	struct Plane {
		Scene* owner{};
		xx::Quad body;
		xx::XY pos{}, inc{};
		float speed{}, frame{}, radius{};
		int64_t level{}, fireCD{}, bulletDamage{}, fireableFrameNumber{}, invincibleFrameNumber{};
		void Init(Scene* owner, xx::XY const& bornPos = {}, int64_t const& invincibleTime_ = 0);
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
		xx::Coro SceneLogic_PlaneReborn(xx::XY deathPos = {}, xx::XY bornPos = {});

		void AddMonster(MonsterBase* m);	// insert into monsters & sync index
		void EraseMonster(MonsterBase* m);	// remove from monsters & clear index

		// res
		std::vector<xx::Shared<xx::Frame>> framesPlane;			// p, p1 ~ 3
		std::vector<xx::Shared<xx::Frame>> framesNumber;		// n0 ~ 9
		std::vector<xx::Shared<xx::Frame>> framesBullet;		// b1 ~ 9
		std::vector<xx::Shared<xx::Frame>> framesRocket;		// r1 ~ 2
		std::vector<xx::Shared<xx::Frame>> framesMonster1;		// ma1 ~ 6
		std::vector<xx::Shared<xx::Frame>> framesMonster2;		// mb1 ~ 5
		std::vector<xx::Shared<xx::Frame>> framesMonster3;		// mc1 ~ 4
		std::vector<xx::Shared<xx::Frame>> framesBackground;	// bg1
		std::vector<xx::Shared<xx::Frame>> framesMonsterBullet;	// eb1 ~ 4
		std::vector<xx::Shared<xx::Frame>> framesEffect;		// e1 ~ 5
		std::vector<xx::Shared<xx::Frame>> framesStuff;			// po1 ~ 3, ph, ps, pc
		std::vector<xx::Shared<xx::Frame>> framesText;			// tstart, tgameover

		std::vector<xx::Shared<xx::MovePathCache>> mpcsMonster;
		// ... mpcsBullet? mpcsStuff?

		// env
		float timePool{};
		float bgScale{}, scale{};
		int64_t frameNumber{};
		xx::XY lastPlanePos{};
		xx::Rnd rnd;
		int stuffIndex{};

		xx::SpaceGridC<MonsterBase> monsterGrid;
		std::vector<MonsterBase*> tmpMonsters;

		Space space;
		Score score;
		xx::Shared<Plane> plane;
		std::vector<xx::Shared<Bullet>> bullets;
		std::vector<xx::Shared<MonsterBase>> monsters;
		std::vector<xx::Shared<LabelEffect>> labels;
		std::vector<xx::Shared<Power>> powers;
		std::vector<xx::Shared<DeathEffect>> deathEffects;
		// ...


		xx::Coros coros;
	};
}
