#pragma once
#ifndef XX2D_TESTS3_MAIN_H_
#define XX2D_TESTS3_MAIN_H_
#include "xx2d.h"
#include "xx_queue.h"

struct Plane;
struct MonsterStrawberry;
struct MonsterDragonfly;
struct MonsterHermitCrab;
struct MonsterFly;
struct ExplosionMonster;

struct GameLooper : xx::GameLooperBase {
	// base res
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;

	// frame update envs
	double timePool{}, nowSecs{};
	xx::Tasks tasks;

	// res
	std::vector<xx::Shared<xx::Frame>> frames_plane_blue;
	std::vector<xx::Shared<xx::Frame>> frames_plane_red;
	std::vector<xx::Shared<xx::Frame>> frames_bullet_plane;
	std::vector<xx::Shared<xx::Frame>> frames_bomb;
	// ... bomb effects

	std::vector<xx::Shared<xx::Frame>> frames_monster_strawberry;
	std::vector<xx::Shared<xx::Frame>> frames_monster_dragonfly;
	std::vector<xx::Shared<xx::Frame>> frames_monster_hermit_crab;
	std::vector<xx::Shared<xx::Frame>> frames_monster_fly;
	// ... more mosters

	std::vector<xx::Shared<xx::Frame>> frames_explosion_monster;
	std::vector<xx::Shared<xx::Frame>> frames_explosion_bigmonster;
	// ... more effects

	// runtime objects
	std::vector<xx::Shared<Plane>> player_planes;

	xx::ListLink<xx::Shared<MonsterStrawberry>, int> monsters_strawberry;
	xx::ListLink<xx::Shared<MonsterDragonfly>, int> monsters_dragonfly;
	xx::ListLink<xx::Shared<MonsterHermitCrab>, int> monsters_hermit_crab;
	xx::ListLink<xx::Shared<MonsterFly>, int> monsters_fly;
	// ... more monsters

	xx::ListLink<xx::Shared<ExplosionMonster>, int> explosions_monster;
	// ... more effects

	// engine event handlers
	void AfterGLInit() override;
	int Update() override;
	xx::Task<> MasterLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

// global runtimes ( for easy use )
inline GameLooper* gLooper;									// fill by main()
inline xx::Engine& gEngine = xx::engine;
inline xx::XY& gMousePos = gEngine.mousePosition;
inline xx::Rnd& gRnd = gEngine.rnd;

// global configs
constexpr float gFps = 120.f;	//60.f;
constexpr double gFds = 1. / gFps;
constexpr float gSpeedScale = 60.f / gFps;

// window size = design size * display scale
constexpr float gDisplayScale = 4.f;					
constexpr float g1_DisplayScale = 1.f / gDisplayScale;

#define STCO static constexpr
struct {
	STCO float width = 224;
	STCO float height = 256;
	STCO float width_2 = width / 2;
	STCO float height_2 = height / 2;
} constexpr gDesign;

/*
	screen design anchor point
   ┌───────────────────────────────┐
   │ 7             8             9 │
   │                               │
   │                               │
   │ 4             5             6 │
   │                               │
   │                               │
   │ 1             2             3 │
   └───────────────────────────────┘
*/
struct {
	STCO float x1 = -gDesign.width_2;
	STCO float y1 = -gDesign.height_2;
	STCO float x2 = 0.f;
	STCO float y2 = -gDesign.height_2;
	STCO float x3 = gDesign.width_2;
	STCO float y3 = -gDesign.height_2;
	STCO float x4 = -gDesign.width_2;
	STCO float y4 = 0.f;
	STCO float x5 = 0.f;
	STCO float y5 = 0.f;
	STCO float x6 = gDesign.width_2;
	STCO float y6 = 0.f;
	STCO float x7 = -gDesign.width_2;
	STCO float y7 = gDesign.height_2;
	STCO float x8 = 0.f;
	STCO float y8 = gDesign.height_2;
	STCO float x9 = gDesign.width_2;
	STCO float y9 = gDesign.height_2;
} constexpr g9Pos;

struct {
	STCO float height = 25.f;
	STCO float height_2 = height / 2;
	STCO float radius = 9.f;
	STCO float bornXs[2] = { g9Pos.x7 + 80 , g9Pos.x1 + 128 };
	STCO float bornYFrom = g9Pos.y2 - height_2;	// out of the screen
	STCO float bornYTo = g9Pos.y7 - 220;
	STCO float bornSpeed = 1.f * gSpeedScale;
	STCO float normalSpeed = 1.5f * gSpeedScale;
	STCO float maxSpeed = 4.f * gSpeedScale;
	STCO float visibleInc = 0.8 * gSpeedScale;
	STCO float speedIncreaseStep = 0.5f * gSpeedScale;	// eat "S" effect
	STCO float frameIndexMin = 0;
	STCO float frameIndexMid = 2;
	STCO float frameIndexMax = 4;
} constexpr gPlane;

struct {
	STCO float hight = 16.f;
	STCO float hight_2 = hight / 2;
	STCO float spacing = 13.f;
	STCO float spacing_2 = spacing / 2;
	STCO float frameChangeStep = 1.f / 5 * gSpeedScale;
	STCO float speed = 5.f * gSpeedScale;
	STCO float fireYOffset = 14.f;
	STCO float fireCD = 1.f / 12;
} constexpr gPlaneBullet;

struct {
	STCO float anchorYDist = 18.f;	// plane.pos.y - offset
	STCO float radius = 6.f;
	STCO float diameter = radius * 2;
	STCO float minSpeed = 0.5f * gSpeedScale;
	STCO float minSpeedPow2 = minSpeed * minSpeed;
	STCO float firstFollowSteps = 2 / gSpeedScale;
	STCO float movingFollowSteps = 9 / gSpeedScale;
	STCO float stopFollowSteps = 6 / gSpeedScale;
} constexpr gBomb;

struct {
    STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
    STCO int frameIndexMin = 0;
    STCO int frameIndexMax = 5;
    STCO int bigFrameIndexMin = 0;
    STCO int bigFrameIndexMax = 4;
} constexpr gExplosionMonster;

struct {
	STCO float radius = 6.f;
	STCO float diameter = radius * 2;
	STCO float bornYFrom = g9Pos.y7 - 96;
	STCO float bornYTo = g9Pos.y7 - 40;
	STCO float horizontalMoveSpeed = 1.5f * gSpeedScale;
	STCO float horizontalMoveFrameSwitchDelay = 1.f / 6 * gSpeedScale;
	STCO int horizontalFrameIndexMin = 0;
	STCO int horizontalFrameIndexMax = 3;
	STCO float horizontalMoveTotalSeconds = (gDesign.width + diameter) / horizontalMoveSpeed / gFps;
	STCO int switchToVerticalMoveDelayFrom = gFps * (horizontalMoveTotalSeconds * 0.2);
	STCO int switchToVerticalMoveDelayTo = gFps * (horizontalMoveTotalSeconds * 0.8);
	STCO float verticalMoveSpeed = 1.f * gSpeedScale;
	STCO float verticalMoveFrameSwitchDelay = 1.f / 4 * gSpeedScale;
	STCO int verticalFrameIndexMin = 4;
	STCO int verticalFrameIndexMax = 5;
	STCO int verticalRepeatFrameIndexMin = 6;
	STCO int verticalRepeatFrameIndexMax = 9;
} constexpr gMonsterStrawberry;

struct {
	STCO float radius = 13.f;
	STCO float diameter = radius * 2;
	STCO float speed = 2.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 3;
	STCO float frameSwitchDelay = 1.f / 5 * gSpeedScale;
} constexpr gMonsterDragonfly;

struct {
	STCO float radius = 5.f;
	STCO float diameter = radius * 2;
	STCO float speed = 3.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 7;
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
} constexpr gMonsterFly;

struct {
	STCO float radius = 5.f;
	STCO float diameter = radius * 2;
	STCO float speed = 1.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 7;
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
} constexpr gMonsterHermitCrab;


/*****************************************************************************************************/
/*****************************************************************************************************/

enum class BombTypes : int {
	Trident,	// bomb0
	Circle,
	Missle,
	Bar,
	Pillar,
	Speed,
	Life,
	Bonus2,
	Bonus5,
	MAX_VALUE_UNKNOWN
};

struct Bomb {
	BombTypes type = BombTypes::MAX_VALUE_UNKNOWN;
	xx::XY pos{};
};

struct PlaneBullet {
	xx::XY pos{};
};

struct Plane {
	int planeIndex{};									// 0: p1, 1: p2

	bool godMode{};										// for born shine 5 secs ( no hit check )	// todo: dead state
	bool moving{};
	float visible{};									// god mode shine delay control

	xx::XY pos{};										// current position
	float speed{};

	float frameIndex{};									// for move left/right switch frame
	float frameChangeSpeed{};							// speed0 / 5

	xx::Queue<Bomb> bombs;								// tail bomb icons
	float bombNextUseTime{};							// next avaliable use bomb time by engine.nowSecs + CD

	xx::ListLink<PlaneBullet> bullets;					// bullets fired from the front of the plane
	float bulletBeginFrameIndex{};						// bullets will be auto switch frame every 5 frames
	float bulletNextFireTime{};							// next avaliable fire time by engine.nowSecs + CD

	void Init(int planeIndex_ = 0);
	void InitBombPos();
	void Draw(xx::Quad& texBrush);

	xx::Task<> Born();									// let the player's plane move in from outside the screen
	xx::Task<> Shine();									// shine 5 secs ( god mode )
	xx::Task<> Update();								// can control move & fire
	xx::Task<> SyncBombPos();							// let tail bombs follow plane move
	xx::Task<> SyncBulletPosCol();						// move bullet & switch frame
	xx::Tasks tasks;									// manager for above tasks
};

template<float radius, bool isBigExplosions, typename MT>
std::optional<xx::XY> HitCheck(PlaneBullet& b, xx::ListLink<xx::Shared<MT>, int>& tar) {
	auto [idx, next] = tar.FindIf([&](xx::Shared<MT>& o)->bool {
		auto d = b.pos - o->pos;
		auto constexpr rr = (gPlaneBullet.hight_2 + radius) * (gPlaneBullet.hight_2 + radius);
		auto dd = d.x * d.x + d.y * d.y;
		if (dd < rr) {
			gLooper->explosions_monster.Emplace().Emplace()->Init(o->pos, isBigExplosions);
			return true;
		}
		return false;
	});
	if (idx != -1) {
		auto r = tar[idx]->pos;
		tar.Remove(idx, next);
		return r;
	}
	return {};
}

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ExplosionMonster {
	xx::XY pos{};
	float frameIndex{};
	std::vector<xx::Shared<xx::Frame>>* frames;
	void Init(xx::XY const& pos_, bool isBig = false);
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update;
	xx::Task<> Update_();
	xx::Task<> UpdateBig_();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct MonsterStrawberry {
	xx::XY pos{}, inc{};
	int switchDelay{};
	float frameIndex{};
	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update = Update_();
	xx::Task<> Update_();
};

struct MonsterDragonfly {
	xx::XY pos{};
	float frameIndex{}, totalDistance{};
	xx::MovePathCache* path;
	void Init(xx::MovePathCache* path_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update = Update_();
	xx::Task<> Update_();
};

struct MonsterHermitCrab {
	xx::XY pos{};
	float frameIndex{}, totalDistance{};
	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update = Update_();
	xx::Task<> Update_();
};

struct MonsterFly {
	xx::XY pos{};
	float frameIndex{}, totalDistance{};
	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update = Update_();
	xx::Task<> Update_();
};

#endif
