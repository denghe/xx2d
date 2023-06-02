#pragma once
#ifndef XX2D_TESTS_MAIN_H_
#define XX2D_TESTS_MAIN_H_

#include "xx2d.h"
#include "xx2d_fps_viewer.h"
#include <xx_lua_data.h>

struct GameLooper : xx::GameLooperBase {
	//xx::BMFont fontBase, font3500;
	//xx::FpsViewer fpsViewer;
	xx::Lua::StateWithExtra<int> L;

	//xx::Shared<xx::GLTexture> tex;
	//xx::ListDoubleLink<xx::Quad> quads;
	//xx::Coros coros;
	//double timePool{};
	//xx::Coro QuadLogic();

	void AfterGLInit() override;
	int Update() override;
};

#endif
