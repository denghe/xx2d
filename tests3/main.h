#pragma once
#ifndef XX2D_TESTS3_MAIN_H_
#define XX2D_TESTS3_MAIN_H_
#include "xx2d.h"
#include "xx_queue.h"

struct Plane;
struct ExplosionMonster;
struct ExplosionBigMonster;
struct Bomb;
struct MonsterStrawberry;
struct MonsterDragonfly;
struct MonsterHermitCrab;
struct MonsterFly;
struct MonsterBigFly;
struct MonsterButterfly;
struct MonsterClip;

struct GameLooper : xx::GameLooperBase {
	// base res
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;

	// frame update envs
	double timePool{}, nowSecs{};
	xx::Tasks tasks;

	// res
	std::vector<xx::Shared<xx::Frame>> frames_plane_blue
		, frames_plane_red
		, frames_bullet_plane
	    , frames_bomb
	// ... bomb effects
	    , frames_monster_strawberry
	    , frames_monster_dragonfly
	    , frames_monster_hermit_crab
	    , frames_monster_fly
	    , frames_monster_bigfly
	    , frames_monster_butterfly
	    , frames_monster_clip
	// ... more mosters
	    , frames_explosion_monster
	    , frames_explosion_bigmonster;
	// ... more effects

	// runtime objects
	std::vector<xx::Shared<Plane>> player_planes;

	xx::ListLink<xx::Shared<Bomb>, int> bombs;

	xx::ListLink<xx::Shared<MonsterStrawberry>, int> monsters_strawberry;
	xx::ListLink<xx::Shared<MonsterDragonfly>, int> monsters_dragonfly;
	xx::ListLink<xx::Shared<MonsterHermitCrab>, int> monsters_hermit_crab;
	xx::ListLink<xx::Shared<MonsterFly>, int> monsters_fly;
	xx::ListLink<xx::Shared<MonsterBigFly>, int> monsters_bigfly;
	xx::ListLink<xx::Shared<MonsterButterfly>, int> monsters_butterfly;
	xx::ListLink<xx::Shared<MonsterClip>, int> monsters_clip;
	// ... more monsters

	xx::ListLink<xx::Shared<ExplosionMonster>, int> explosions_monster;
	xx::ListLink<xx::Shared<ExplosionBigMonster>, int> explosions_bigmonster;
	// ... more effects

	// utils
	xx::XY const* GetPlanePos() {
		if (player_planes.empty()) return {};
		return &player_planes[0]->pos;	// todo: random index?
	}

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

struct {
	static constexpr float width = 224;
	static constexpr float height = 256;
	static constexpr float width_2 = width / 2;
	static constexpr float height_2 = height / 2;
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
	static constexpr float x1 = -gDesign.width_2;
	static constexpr float y1 = -gDesign.height_2;
	static constexpr float x2 = 0.f;
	static constexpr float y2 = -gDesign.height_2;
	static constexpr float x3 = gDesign.width_2;
	static constexpr float y3 = -gDesign.height_2;
	static constexpr float x4 = -gDesign.width_2;
	static constexpr float y4 = 0.f;
	static constexpr float x5 = 0.f;
	static constexpr float y5 = 0.f;
	static constexpr float x6 = gDesign.width_2;
	static constexpr float y6 = 0.f;
	static constexpr float x7 = -gDesign.width_2;
	static constexpr float y7 = gDesign.height_2;
	static constexpr float x8 = 0.f;
	static constexpr float y8 = gDesign.height_2;
	static constexpr float x9 = gDesign.width_2;
	static constexpr float y9 = gDesign.height_2;
} constexpr g9Pos;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct PosFrameIndexTasks : xx::Tasks {
	xx::XY pos{};
	float frameIndex{};
	bool disposing{};
};

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

struct PlaneBomb {
	BombTypes type = BombTypes::MAX_VALUE_UNKNOWN;
	xx::XY pos{};
};

struct PlaneBullet {
	static constexpr float hight = 16.f;
	static constexpr float hight_2 = hight / 2;
	static constexpr float spacing = 13.f;
	static constexpr float spacing_2 = spacing / 2;
	static constexpr float radius = spacing_2;
	static constexpr float frameSwitchDelay = 1.f / 5 * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 3;
	static constexpr float speed = 5.f * gSpeedScale;
	static constexpr float fireYOffset = 14.f;
	static constexpr float fireCD = 1.f / 12;

	xx::XY pos{};
};

struct Plane : PosFrameIndexTasks {
	static constexpr float height = 25.f;
	static constexpr float height_2 = height / 2;
	static constexpr float radius = 9.f;
	static constexpr float bornXs[2] = { g9Pos.x7 + 80 , g9Pos.x1 + 128 };
	static constexpr float bornYFrom = g9Pos.y2 - height_2;	// out of the screen
	static constexpr float bornYTo = g9Pos.y7 - 220;
	static constexpr float bornSpeed = 1.f * gSpeedScale;
	static constexpr float normalSpeed = 1.5f * gSpeedScale;
	static constexpr float maxSpeed = 4.f * gSpeedScale;
	static constexpr float visibleInc = 0.4 * gSpeedScale;
	static constexpr float speedIncreaseStep = 0.5f * gSpeedScale;	// eat "S" effect
	static constexpr float frameIndexMin = 0;
	static constexpr float frameIndexMid = 2;
	static constexpr float frameIndexMax = 4;

	int planeIndex{};									// 0: p1, 1: p2

	bool godMode{};										// for born shine 5 secs ( no hit check )	// todo: dead state
	bool moving{};
	float visible{};									// god mode shine delay control

	float speed{};


	xx::Queue<PlaneBomb> bombs;							// tail bomb icons
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
};


/*****************************************************************************************************/
/*****************************************************************************************************/

struct Bomb : PosFrameIndexTasks {
	static constexpr float anchorYDist = 18.f;	// plane.pos.y - offset
	static constexpr float radius = 6.f;
	static constexpr float diameter = radius * 2;
	static constexpr float minSpeed = 0.5f * gSpeedScale;
	static constexpr float minSpeedPow2 = minSpeed * minSpeed;
	static constexpr float firstFollowSteps = 2 / gSpeedScale;
	static constexpr float movingFollowSteps = 9 / gSpeedScale;
	static constexpr float stopFollowSteps = 6 / gSpeedScale;

	BombTypes type = BombTypes::MAX_VALUE_UNKNOWN;
	void Init(xx::XY const& pos_, BombTypes type_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct ExplosionMonster : PosFrameIndexTasks {
	static constexpr float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 5;

	void Init(xx::XY const& pos_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct ExplosionBigMonster : PosFrameIndexTasks {
	static constexpr float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 4;

	void Init(xx::XY const& pos_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterStrawberry : PosFrameIndexTasks {
	static constexpr float radius = 6.f;
	static constexpr float diameter = radius * 2;
	static constexpr float bornYFrom = g9Pos.y7 - 96;
	static constexpr float bornYTo = g9Pos.y7 - 40;
	static constexpr float frameSwitchDelay = 1.f / 6 * gSpeedScale;
	static constexpr float horizontalMoveSpeed = 1.5f * gSpeedScale;
	static constexpr int horizontalFrameIndexMin = 0;
	static constexpr int horizontalFrameIndexMax = 3;
	static constexpr float horizontalMoveTotalSeconds = (gDesign.width + diameter) / horizontalMoveSpeed / gFps;
	static constexpr int switchToVerticalMoveDelayFrom = gFps * (horizontalMoveTotalSeconds * 0.2);
	static constexpr int switchToVerticalMoveDelayTo = gFps * (horizontalMoveTotalSeconds * 0.8);
	static constexpr float verticalMoveSpeed = 1.f * gSpeedScale;
	static constexpr float verticalMoveFrameSwitchDelay = 1.f / 4 * gSpeedScale;
	static constexpr int verticalFrameIndexMin = 4;
	static constexpr int verticalFrameIndexMax = 5;
	static constexpr int verticalRepeatFrameIndexMin = 6;
	static constexpr int verticalRepeatFrameIndexMax = 9;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterDragonfly : PosFrameIndexTasks {
	static constexpr float radius = 13.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speed = 2.f * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 3;
	static constexpr float frameSwitchDelay = 1.f / 5 * gSpeedScale;
	inline static xx::MovePathCache paths[2];

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterHermitCrab : PosFrameIndexTasks {
	static constexpr float radius = 6.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speed = 1.f * gSpeedScale;
	static constexpr float bornPosY = g9Pos.y7 + diameter;
	static constexpr float bornPosXFrom = g9Pos.x7 + 20;
	static constexpr float bornPosXTo = g9Pos.x9 - 20;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 7;
	static constexpr float frameSwitchDelay = 1.f / 4 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterFly : PosFrameIndexTasks {
	static constexpr float radius = 7.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speedMin = 0.5f * gSpeedScale;
	static constexpr float speedMax = 2.f * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 7;
	static constexpr float frameSwitchDelay = 1.f / 3 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterBigFly : PosFrameIndexTasks {
	static constexpr float radius = 8.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speed = 1.f * gSpeedScale;
	static constexpr float x1 = g9Pos.x7 + 33;
	static constexpr float x2 = g9Pos.x9 - 33;
	static constexpr float horizontalMoveToY = g9Pos.y7 - 146;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 5;
	static constexpr float frameSwitchDelay1 = 1.f / 8 * gSpeedScale;	// horizontal move
	static constexpr float frameSwitchDelay2 = 1.f / 2 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterButterfly : PosFrameIndexTasks {		// todo
	static constexpr float radius = 5.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speed = 3.f * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 3;
	static constexpr float frameSwitchDelay = 1.f / 4 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

struct MonsterClip : PosFrameIndexTasks {
	static constexpr float radius = 4.f;
	static constexpr float diameter = radius * 2;
	static constexpr float speed = 1.f * gSpeedScale;
	static constexpr float xFrom = g9Pos.x7 + diameter * 2;
	static constexpr float xTo = g9Pos.x9 - diameter * 2;
	static constexpr float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	static constexpr int frameIndexMin = 0;
	static constexpr int frameIndexMax = 3;
	static constexpr float aimPosYOffset = -Plane::height_2 / 2;
	static constexpr float aimDelaySeconds = 1.f / 60 * 4;
	static constexpr int verticalFrameIndexMin = 4;	// right to left
	static constexpr int verticalFrameIndexMax = 7;
	static constexpr int verticalFrameIndexMin2 = 8;	// left to right
	static constexpr int verticalFrameIndexMax2 = 11;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> MainLogic();
};

#endif
