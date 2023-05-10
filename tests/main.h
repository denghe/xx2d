#pragma once
#include "xx2d.h"
#include "xx2d_fps_viewer.h"
#include <xx_lua_data.h>
namespace xL = xx::Lua;

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase, font3500;
	xx::FpsViewer fpsViewer;
	xx::Coros coros;
	xL::State L;
	void Init() override;
	int Update() override;
};
