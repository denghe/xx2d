#pragma once
#ifndef XX2D_TESTS3_MAIN_H_
#define XX2D_TESTS3_MAIN_H_
#include "xx2d.h"
#include "xx_queue.h"

struct Plane;
struct MonsterStrawberry;

struct GameLooper : xx::GameLooperBase {
	// base res
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;

	// frame update envs
	double timePool{};
	int64_t frameNumber{};
	xx::Tasks tasks;

	// res
	std::vector<xx::Shared<xx::Frame>> frames_plane_blue;
	std::vector<xx::Shared<xx::Frame>> frames_plane_red;
	std::vector<xx::Shared<xx::Frame>> frames_bullet_plane;
	std::vector<xx::Shared<xx::Frame>> frames_bomb;
	// ... bomb effects

	std::vector<xx::Shared<xx::Frame>> frames_monster_strawberry;
	// ... more mosters

	// ... more ffects

	// runtime objects
	std::vector<xx::Shared<Plane>> player_planes;

	xx::ListDoubleLink< xx::Shared<MonsterStrawberry>, int, uint32_t> monster_strawberries;
	// ... more monsters

	// engine event handlers
	void AfterGLInit() override;
	int Update() override;
	xx::Task<> MasterLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

// global configs ( for easy use )
inline GameLooper* gLooper;									// fill by main()
inline xx::Engine& gEngine = xx::engine;
inline xx::XY& gMousePos = gEngine.mousePosition;
inline xx::Rnd& gRnd = gEngine.rnd;

// following speed mean: position increase value by every frame
inline constexpr float gFps = 120.f;	//60.f;
inline constexpr float gFds = 1.f / gFps;
inline constexpr float gSpeedScale = 60.f / gFps;
inline constexpr float gDisplayScale = 4;					// upscale pixel style textures
inline constexpr float g1_Scale = 1. / 4;
inline constexpr float gDesignWidth = 224;
inline constexpr float gDesignHeight = 256;
inline constexpr float gDesignWidth_2 = gDesignWidth / 2;
inline constexpr float gDesignHeight_2 = gDesignHeight / 2;
inline constexpr float gWndWidth = gDesignWidth * gDisplayScale;
inline constexpr float gWndHeight = gDesignHeight * gDisplayScale;
inline constexpr float gWndWidth_2 = gWndWidth / 2;
inline constexpr float gWndHeight_2 = gWndHeight / 2;

inline constexpr float gPlaneHeight = 25.f * gDisplayScale;
inline constexpr float gPlaneHeight_2 = gPlaneHeight / 2;
inline constexpr float gPlaneRadius = 9.f * gDisplayScale;
inline constexpr float gPlaneBornXs[2] = { 80 * gDisplayScale - gWndWidth_2, 128 * gDisplayScale - gWndWidth_2 };
inline constexpr float gPlaneBornYFrom = -gWndHeight_2 - gPlaneHeight_2;
inline constexpr float gPlaneBornYTo = gWndHeight_2 - 220 * gDisplayScale;
inline constexpr float gPlaneBornSpeed = 1.f * gSpeedScale;
inline constexpr float gPlaneNormalSpeed = 1.5f * gSpeedScale;
inline constexpr float gPlaneMaxSpeed = 4.f * gSpeedScale;
inline constexpr float gPlaneVisibleInc = 0.8 * gSpeedScale;
inline constexpr float gPlaneSpeedIncreaseStep = 0.5f * gSpeedScale;	// eat "S" effect
inline constexpr float gPlaneFrameIndexMin = 0;
inline constexpr float gPlaneFrameIndexMid = 2;
inline constexpr float gPlaneFrameIndexMax = 4;

inline constexpr float gPlaneBulletHight = 16.f * gDisplayScale;
inline constexpr float gPlaneBulletHight_2 = gPlaneBulletHight / 2;
inline constexpr float gPlaneBulletSpacing = 13.f * gDisplayScale;
inline constexpr float gPlaneBulletSpacing_2 = gPlaneBulletSpacing / 2;
inline constexpr float gPlaneBulletFrameChangeStep = 1.f / 5;
inline constexpr float gPlaneBulletSpeed = 5.f * gDisplayScale * gSpeedScale;
inline constexpr float gPlaneBulletFireYOffset = 14.f * gDisplayScale;
inline constexpr float gPlaneBulletFireCD = 1.f / 10;

inline constexpr float gBombAnchorYDist = 18.f * gDisplayScale;	// plane.pos.y - offset
inline constexpr float gBombRadius = 6.f * gDisplayScale;
inline constexpr float gBombDiameter = gBombRadius * 2;
inline constexpr float gBombMinSpeed = 0.5f * gDisplayScale * gSpeedScale;
inline constexpr float gBombMinSpeedPow2 = gBombMinSpeed * gBombMinSpeed;
inline constexpr float gBombFirstFollowSteps = 2 / gSpeedScale;
inline constexpr float gBombMovingFollowSteps = 9 / gSpeedScale;
inline constexpr float gBombStopFollowSteps = 6 / gSpeedScale;

inline constexpr float gMonsterStrawberryRadius = 7.f * gDisplayScale;
inline constexpr float gMonsterStrawberryDiameter = gMonsterStrawberryRadius * 2;
inline constexpr float gMonsterStrawberryBornYFrom = gWndHeight_2 - 96 * gDisplayScale;
inline constexpr float gMonsterStrawberryBornYTo = gWndHeight_2 - 40 * gDisplayScale;
inline constexpr float gMonsterStrawberryHorizontalMoveSpeed = 1.5f * gDisplayScale * gSpeedScale;
inline constexpr float gMonsterStrawberryHorizontalMoveFrameSwitchDelay = 1.f / (60 / 6);
inline constexpr int gMonsterStrawberryHorizontalFrameIndexMin = 0;
inline constexpr int gMonsterStrawberryHorizontalFrameIndexMax = 3;
inline constexpr float gMonsterStrawberryHorizontalMoveTotalSeconds = (gWndWidth + gMonsterStrawberryDiameter) / gMonsterStrawberryHorizontalMoveSpeed / gFps;
inline constexpr int gMonsterStrawberrySwitchToVerticalMoveDelayFrom = gFps * (gMonsterStrawberryHorizontalMoveTotalSeconds * 0.2);
inline constexpr int gMonsterStrawberrySwitchToVerticalMoveDelayTo = gFps * (gMonsterStrawberryHorizontalMoveTotalSeconds * 0.8);
inline constexpr float gMonsterStrawberryVerticalMoveSpeed = 1.f * gDisplayScale * gSpeedScale;
inline constexpr float gMonsterStrawberryVerticalMoveFrameSwitchDelay = 1.f / (60 / 4);
inline constexpr int gMonsterStrawberryVerticalFrameIndexMin = 4;
inline constexpr int gMonsterStrawberryVerticalFrameIndexMax = 5;
inline constexpr int gMonsterStrawberryVerticalRepeatFrameIndexMin = 6;
inline constexpr int gMonsterStrawberryVerticalRepeatFrameIndexMax = 9;


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
	float radius{};										// for collision detect

	xx::XY pos{};										// current position
	float speed0{};										// base speed ( for calc )
	float speed1{};										// speed0 * gDisplayScale

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

struct MonsterStrawberry {
	xx::XY pos{}, inc{};
	int switchDelay{};
	float frameIndex{};
	void Init();
	void Draw(xx::Quad& texBrush);
	xx::Task<> Update = Update_();
	xx::Task<> Update_();
};

#endif
