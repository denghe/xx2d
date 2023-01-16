#include "pch.h"

void Engine::EngineInit() {
	lastTime = xx::NowSteadyEpochSeconds();
	lastDelta = 0;
	SearchPathReset();

	sm.Init(this);
}

void Engine::EngineUpdateBegin() {
	lastDelta = xx::NowSteadyEpochSeconds(lastTime);
	sm.Begin();

	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);
	glDepthMask(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(false);
}

void Engine::EngineUpdateEnd() {
	sm.End();
}

void Engine::EngineDestroy() {
	textureCache.clear();
	// ...
}

void Engine::SetWH(float w, float h) {
	this->w = w;
	this->h = h;
	hw = (float)w / 2;
	hh = (float)h / 2;
	ninePoints[1] = { -hw, -hh };
	ninePoints[2] = { 0, -hh };
	ninePoints[3] = { hw, -hh };
	ninePoints[4] = { -hw, 0 };
	ninePoints[5] = { 0, 0 };
	ninePoints[6] = { hw, 0 };
	ninePoints[7] = { -hw, hh };
	ninePoints[8] = { 0, hh };
	ninePoints[9] = { hw, hh };
}
