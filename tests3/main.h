#pragma once
#ifndef XX2D_TESTS3_MAIN_H_
#define XX2D_TESTS3_MAIN_H_
#include "xx2d.h"

struct Plane;
struct GameLooper : xx::GameLooperBase {
	// base res
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;

	// frame update envs
	double timePool{};
	int64_t frameNumber{};
	xx::Tasks tasks;

	// res
	std::vector<xx::Shared<xx::Frame>> frames_plane_blue;			// plane_b?     ? = 1~5
	std::vector<xx::Shared<xx::Frame>> frames_plane_red;			// plane_r?     ? = 1~5

	// runtime objects
	std::vector<xx::Shared<Plane>> player_planes;

	// engine event handlers
	void AfterGLInit() override;
	int Update() override;
	xx::Task<> MasterLogic();
};

// global configs ( for easy use )
inline constexpr float gScale = 4;					// upscale pixel style textures
inline constexpr float g1_Scale = 1. / 4;
inline constexpr float gWndWidth = 224 * gScale;
inline constexpr float gWndHeight = 256 * gScale;
inline constexpr float gWndWidth_2 = gWndWidth / 2;
inline constexpr float gWndHeight_2 = gWndHeight / 2;
inline constexpr float gFps = 60.f;
inline constexpr float gFds = 1.f / gFps;

inline GameLooper* gLooper;							// fill by main()
inline xx::Engine& gEngine = xx::engine;
inline xx::XY& gMousePos = gEngine.mousePosition;

inline constexpr float gPlaneHeight = 25.f * gScale;
inline constexpr float gPlaneRadius = 9.f * gScale;
inline constexpr float gPlaneBornXs[2] = { 80 * gScale - gWndWidth_2, 128 * gScale - gWndWidth_2 };
inline constexpr float gPlaneBornYFrom = -gWndHeight_2 - gPlaneHeight / 2;
inline constexpr float gPlaneBornYTo = gWndHeight_2 - 220 * gScale;
inline constexpr float gPlaneBornSpeed = 1.f;
inline constexpr float gPlaneNormalSpeed = 1.5f;

struct Plane {
	xx::Quad body;									// texture brush

	int planeIndex{};								// 0: p1, 1: p2
	std::vector<xx::Shared<xx::Frame>>* frames{};	// planeIndex == 0: frames_plane_blue; == 1: frames_plane_red

	bool godMode{ true }, visible{};				// for born shine 5 secs
	float radius{};									// for collision detect

	xx::XY pos{};									// current position
	float speed0{};									// base speed
	float speed1{};									// speed0 * gScale

	float frameIndexs[4];							// for move left/right switch frame. [0] curr [1] min [2] mid [3] max
	float frameChangeSpeed{};						// speed0 / 5

	xx::Tasks tasks;

	void Init(int planeIndex_ = 0);
	void Draw();
	xx::Task<> Born();								// let the player's plane move in from outside the screen
	xx::Task<> Shine();								// shine 5 secs ( god mode )
	xx::Task<> Update();							// can control move & fire
};

#endif
