#pragma once
#include "xx2d.h"
#include "xx2d_fps_viewer.h"

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase, font3500;
	xx::FpsViewer fpsViewer;
	xx::Coros coros;
	void Init() override;
	int Update() override;
};
