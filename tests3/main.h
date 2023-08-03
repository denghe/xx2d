#pragma once
#ifndef XX2D_TESTS3_MAIN_H_
#define XX2D_TESTS3_MAIN_H_
#include "xx2d.h"

struct Monster;
struct GameLooper : xx::GameLooperBase {
	xx::BMFont fontBase;
	xx::FpsViewer fpsViewer;
	xx::TP tp;

	static constexpr float fps = 60.f, fds = 1.f / fps;
	double timePool{};

	void AfterGLInit() override;
	int Update() override;
};

#endif
