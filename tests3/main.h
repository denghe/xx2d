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

struct PlaneBomb {
	BombTypes type = BombTypes::MAX_VALUE_UNKNOWN;
	xx::XY pos{};
};

struct PlaneBullet {
	STCO float hight = 16.f;
	STCO float hight_2 = hight / 2;
	STCO float spacing = 13.f;
	STCO float spacing_2 = spacing / 2;
	STCO float radius = spacing_2;
	STCO float frameSwitchDelay = 1.f / 5 * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 3;
	STCO float speed = 5.f * gSpeedScale;
	STCO float fireYOffset = 14.f;
	STCO float fireCD = 1.f / 12;

	xx::XY pos{};
};

struct Plane {
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

	int planeIndex{};									// 0: p1, 1: p2

	bool godMode{};										// for born shine 5 secs ( no hit check )	// todo: dead state
	bool moving{};
	float visible{};									// god mode shine delay control

	xx::XY pos{};										// current position
	float speed{};

	float frameIndex{};									// for move left/right switch frame

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
	xx::Tasks tasks;									// manager for above tasks
};

struct PosFrameIndexUpdate {
	xx::XY pos{};
	float frameIndex{};
	xx::Task<> Update;
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Bomb : PosFrameIndexUpdate {
	STCO float anchorYDist = 18.f;	// plane.pos.y - offset
	STCO float radius = 6.f;
	STCO float diameter = radius * 2;
	STCO float minSpeed = 0.5f * gSpeedScale;
	STCO float minSpeedPow2 = minSpeed * minSpeed;
	STCO float firstFollowSteps = 2 / gSpeedScale;
	STCO float movingFollowSteps = 9 / gSpeedScale;
	STCO float stopFollowSteps = 6 / gSpeedScale;

	BombTypes type = BombTypes::MAX_VALUE_UNKNOWN;
	void Init(xx::XY const& pos_, BombTypes type_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct ExplosionMonster : PosFrameIndexUpdate {
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 5;

	void Init(xx::XY const& pos_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct ExplosionBigMonster : PosFrameIndexUpdate {
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 4;

	void Init(xx::XY const& pos_);
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterStrawberry : PosFrameIndexUpdate {
	STCO float radius = 6.f;
	STCO float diameter = radius * 2;
	STCO float bornYFrom = g9Pos.y7 - 96;
	STCO float bornYTo = g9Pos.y7 - 40;
	STCO float frameSwitchDelay = 1.f / 6 * gSpeedScale;
	STCO float horizontalMoveSpeed = 1.5f * gSpeedScale;
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

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterDragonfly : PosFrameIndexUpdate {
	STCO float radius = 13.f;
	STCO float diameter = radius * 2;
	STCO float speed = 2.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 3;
	STCO float frameSwitchDelay = 1.f / 5 * gSpeedScale;
	inline static xx::MovePathCache paths[2];

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterHermitCrab : PosFrameIndexUpdate {
	STCO float radius = 6.f;
	STCO float diameter = radius * 2;
	STCO float speed = 1.f * gSpeedScale;
	STCO float bornPosY = g9Pos.y7 + diameter;
	STCO float bornPosXFrom = g9Pos.x7 + 20;
	STCO float bornPosXTo = g9Pos.x9 - 20;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 7;
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterFly : PosFrameIndexUpdate {
	STCO float radius = 7.f;
	STCO float diameter = radius * 2;
	STCO float speedMin = 0.5f * gSpeedScale;
	STCO float speedMax = 2.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 7;
	STCO float frameSwitchDelay = 1.f / 3 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterBigFly : PosFrameIndexUpdate {
	STCO float radius = 8.f;
	STCO float diameter = radius * 2;
	STCO float speed = 1.f * gSpeedScale;
	STCO float x1 = g9Pos.x7 + 33;
	STCO float x2 = g9Pos.x9 - 33;
	STCO float horizontalMoveToY = g9Pos.y7 - 146;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 5;
	STCO float frameSwitchDelay1 = 1.f / 8 * gSpeedScale;	// horizontal move
	STCO float frameSwitchDelay2 = 1.f / 2 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterButterfly : PosFrameIndexUpdate {		// todo
	STCO float radius = 5.f;
	STCO float diameter = radius * 2;
	STCO float speed = 3.f * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 3;
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

struct MonsterClip : PosFrameIndexUpdate {
	STCO float radius = 4.f;
	STCO float diameter = radius * 2;
	STCO float speed = 1.f * gSpeedScale;
	STCO float xFrom = g9Pos.x7 + diameter * 2;
	STCO float xTo = g9Pos.x9 - diameter * 2;
	STCO float frameSwitchDelay = 1.f / 4 * gSpeedScale;
	STCO int frameIndexMin = 0;
	STCO int frameIndexMax = 3;
	STCO float aimPosYOffset = -Plane::height_2 / 2;
	STCO float aimDelaySeconds = 1.f / 60 * 4;
	STCO int verticalFrameIndexMin = 4;	// right to left
	STCO int verticalFrameIndexMax = 7;
	STCO int verticalFrameIndexMin2 = 8;	// left to right
	STCO int verticalFrameIndexMax2 = 11;

	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update_();
};

#endif
