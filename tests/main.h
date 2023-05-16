#pragma once
#include "xx2d.h"
#include "xx2d_fps_viewer.h"
#include <xx_lua_data.h>

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase, font3500;
	xx::FpsViewer fpsViewer;
	xx::Lua::State L;

	xx::Shared<xx::GLTexture> tex;
	xx::ListDoubleLink<xx::Quad> quads;
	xx::Coros coros;
	double timePool{};
	xx::Coro QuadLogic();

	void Init() override;
	int Update() override;
};
